#ifndef __RESULT_H__
#define __RESULT_H__

#include <variant>

template <typename T, typename E>
class Result
{
    std::variant<T, E> result;

public:
    Result(T value) : result(std::move(value)) {}

    Result(E error) : result(std::move(error)) {}

    bool isOk() const
    {
        return std::holds_alternative<T>(result);
    }

    bool isError() const
    {
        return std::holds_alternative<E>(result);
    }

    T value()
    {
        if (!isOk())
            throw std::bad_variant_access();
        return std::move(std::get<T>(result));
    }

    const E &error() const
    {
        if (!isError())
            throw std::bad_variant_access();
        return std::get<E>(result);
    }
};

#endif