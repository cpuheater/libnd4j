//
// Created by raver119 on 06.11.2017.
//

#include <ops/declarable/CustomOperations.h>

namespace nd4j {
    namespace ops {
        LIST_OP_IMPL(pick_list, 1, 1, 0, -2) {
            auto list = INPUT_LIST(0);

            std::vector<int> indices;
            if (block.width() > 1 && block.getVariable(1)->getNDArray()->isVector()) {
                auto ia = INPUT_VARIABLE(1);
                for (int e = 0; e < ia->lengthOf(); e++)
                    indices.emplace_back((int) ia->getIndexedScalar(e));
            } else if (block.getIArguments()->size() > 0) {
                indices = *(block.getIArguments());
            } else return ND4J_STATUS_BAD_ARGUMENTS;

            for (auto& v: indices)
                if (v >= list->height()) {
                    nd4j_printf("Requested index [%i] is higher (or equal) then ArrayList height: [%i]", v, list->height());
                    return ND4J_STATUS_BAD_ARGUMENTS;
                }

            auto result = list->pick(indices);

            OVERWRITE_RESULT(result);

            return ND4J_STATUS_OK;
        }
    }
}