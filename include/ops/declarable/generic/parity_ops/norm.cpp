//
//  @author raver119@gmail.com
//

#include <ops/declarable/CustomOperations.h>

namespace nd4j {
    namespace ops {
        REDUCTION_OP_IMPL(norm, 1, 1, false, 1, -2) {
            auto input = INPUT_VARIABLE(0);
            NDArray<T> *output = nullptr;

            int mode = (int) T_ARG(0);
            std::vector<int> dims = *block.getIArguments();
            bool overwrite = false;

            if (block.width() == 1) {
                output = OUTPUT_VARIABLE(0);
            } else {
                auto vector = INPUT_VARIABLE(1);

                for (int e = 0; e < vector->lengthOf(); e++) {
                    int ca = (int) vector->getScalar(e);
                    if (ca < 0)
                        ca += input->rankOf();

                    dims.emplace_back(ca);
                }

                int* shape = ShapeUtils<T>::evalReduceShapeInfo(input->ordering(), dims, *input, false);
                output = new NDArray<T>(shape, false, block.getWorkspace());

                overwrite = true;
                RELEASE(shape, input->getWorkspace());
            }

            switch(mode) {
                case 0: {
                    REQUIRE_TRUE(dims.size() == 2 || (input->rankOf() == 2 && dims.size() == 0), 0, "Norm: Frobenius is defined for 2D matrices or TADS only");
                    // fro
                    input->template reduceAlongDimension<simdOps::NormFrobenius<T>>(output, dims);
                }
                break;
                case 1: {
                    // euclidean
                    if ((input->rankOf() == 2 && dims.size() == 0) || dims.size() == 2) {
                        input->template reduceAlongDimension<simdOps::NormFrobenius<T>>(output, dims);
                    } else {
                        input->template reduceAlongDimension<simdOps::Norm2<T>>(output, dims);
                    }
                }
                break;
                case 2: {
                    // 1
                    input->template reduceAlongDimension<simdOps::Norm1<T>>(output, dims);
                }
                break;
                case 3: {
                    // 2 
                    input->template reduceAlongDimension<simdOps::Norm2<T>>(output, dims);
                }
                break;
                case 4: {
                    // inf-norm
                    input->template reduceAlongDimension<simdOps::NormMax<T>>(output, dims);
                }
                break;
                default: {
                    // p-norm
                    REQUIRE_TRUE(block.getIArguments()->size() > 1, 0, "P-Norm reductions requires 2 TArguments, but only 1 was provided");
                    T p = T_ARG(1);
                    input->template reduceAlongDimension<simdOps::NormP<T>>(output, dims, false, &p);
                }
            }

            if (overwrite) {
                OVERWRITE_RESULT(output);
            }

            return ND4J_STATUS_OK;
        };
    }
}