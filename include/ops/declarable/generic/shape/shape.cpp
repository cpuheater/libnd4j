//
//  @author raver119@gmail.com
//

#include <ops/declarable/CustomOperations.h>

namespace nd4j {
    namespace ops {
        CUSTOM_OP_IMPL(shape_of, 1, 1, false, 0, 0) {
            auto x = INPUT_VARIABLE(0);
            auto z = OUTPUT_VARIABLE(0);

            for (int e = 0; e < x->rankOf(); e++)
                z->putIndexedScalar(e, x->sizeAt(e));

            STORE_RESULT(z);

            return ND4J_STATUS_OK;
        };
        DECLARE_SYN(shape, shape_of);

        DECLARE_SHAPE_FN(shape_of) {
            auto inShape = inputShape->at(0);

            int *newshape;
            ALLOCATE(newshape, block.getWorkspace(), shape::shapeInfoLength(1), int);
            nd4j::ShapeBuilder::shapeVector(shape::rank(inShape), newshape);

            return new ShapeList(newshape);
        };
    }
}