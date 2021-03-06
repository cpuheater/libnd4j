//
//  @author raver119@gmail.com
//

#include <ops/declarable/headers/parity_ops.h>

namespace nd4j {
namespace ops {
    CUSTOM_OP_IMPL(split_v, 2, -1, false, 0, -2) {
        auto input = INPUT_VARIABLE(0);
        auto sizes = INPUT_VARIABLE(1);

        int axis = 0;

        if (block.getIArguments()->size() > 0) {
            axis = INT_ARG(0);
        } else if (block.width() > 2){
            auto _a = INPUT_VARIABLE(2);
            axis = _a->getScalar(0);
        } 

        if (axis < 0)
            axis += input->rankOf();

        std::vector<int> dims = ShapeUtils<T>::convertAxisToTadTarget(input->rankOf(), {axis});
        //auto tads = NDArrayFactory<T>::allTensorsAlongDimension(input, dims);

        int pos = 0;
        for (int e = 0; e < sizes->lengthOf(); e++) {
            int c_size = (int) sizes->getScalar(e);
            IndicesList indices;

            for (int d = 0; d < input->rankOf(); d++) {
                if (d == axis) {
                    indices.push_back(NDIndex::interval(pos, pos+c_size));
                } else 
                    indices.push_back(NDIndex::all());
            }

            auto output = OUTPUT_VARIABLE(e);

            auto sub = input->subarray(indices);

            output->assign(sub);

            pos += c_size;
            delete sub;
        }

        //delete tads;
        return ND4J_STATUS_OK;
    }


    DECLARE_SHAPE_FN(split_v) {
        auto input = inputShape->at(0);
        //auto sizes = inputShape->at(1);

        auto shapeList = new ShapeList();
        int rank = shape::rank(input);

        // 0 is just default axis
        int axis = 0;

        if (block.getIArguments()->size() > 0)
            axis = INT_ARG(0);
        else if (block.width() > 2) {
            auto _a = INPUT_VARIABLE(2);
            axis = _a->getScalar(0);
        }

        if (axis < 0)
            axis += shape::rank(input);
        
        // this op assumes we have sizes defined
        auto sizes = INPUT_VARIABLE(1);
        
        auto length = sizes->lengthOf();
        int pos = 0;
        for (int e = 0; e < length; e++) {
            int c_size = sizes->getScalar(e);
            int *newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(input), int);

            std::vector<int> shape(rank);

            for (int d = 0; d < rank; d++) {
                if (d != axis)
                    shape[d] = shape::sizeAt(input, d);
                else 
                    shape[d] = c_size;
            }

            if (shape::order(input) == 'c')
                shape::shapeBuffer(shape.size(), shape.data(), newShape);
            else
                shape::shapeBufferFortran(shape.size(), shape.data(), newShape);

            shapeList->push_back(newShape);
        }

        return shapeList;
    }
}
}