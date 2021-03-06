//
// @author raver119@gmail.com
//

#include <array/ResultSet.h>
#include <graph/FlatUtils.h>

namespace nd4j {
    template <typename T>
    ResultSet<T>::ResultSet(const nd4j::graph::FlatResult* result) {
        if (result != nullptr) {
            for (int e = 0; e < result->variables()->size(); e++) {
                auto var = result->variables()->Get(e);

                NDArray<T>* array;

                if (var->ndarray() != nullptr) {
                    array = nd4j::graph::FlatUtils::fromFlatArray<T>(var->ndarray());
                } else if (var->shape() != nullptr) {
                    std::vector<int> shapeInfo;
                    for (int i = 0; i < var->shape()->size(); i++) {
                        shapeInfo.emplace_back(var->shape()->Get(i));
                    }

                    // we just create empty array here
                    std::vector<int> shape;
                    for (int i = 0; i < shapeInfo.at(0); i++) {
                        shape.emplace_back(shapeInfo.at(i + 1));
                    }

                    array = new NDArray<T>((char) shapeInfo.at(shapeInfo.size() - 1), shape);
                } else {
                    nd4j_printf("Either shape or NDArray should be defined in FlatResult variable\n","");
                    throw "Empty variable";
                }


                _content.push_back(array);
            }
        }
    }

    template <typename T>
    ResultSet<T>::~ResultSet() {
        if (_removable)
            for (auto v: _content)
                delete v;
    }

    template <typename T>
    void ResultSet<T>::setNonRemovable() {
        _removable = false;
    }

    template <typename T>
    int ResultSet<T>::size() {
        return (int) _content.size();
    }

    template <typename T>
    nd4j::NDArray<T>* ResultSet<T>::at(unsigned long idx) {
        return _content.at(idx);
    }

    template <typename T>
    void ResultSet<T>::push_back(nd4j::NDArray<T> *array) {
        _content.emplace_back(array);
    }

    template <typename T>
    Nd4jStatus ResultSet<T>::status() {
        return _status;
    }

    template <typename T>
    void ResultSet<T>::setStatus(Nd4jStatus status) {
        _status = status;
    }

    template <typename T>
    void ResultSet<T>::purge() {
        _content.clear();
    }

    template class ND4J_EXPORT ResultSet<float>;
    template class ND4J_EXPORT ResultSet<float16>;
    template class ND4J_EXPORT ResultSet<double>;
}

