#pragma once

#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if (size != 0) {
            raw_ptr_ = new Type[size];
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;
    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;
    //Лучше добавить move семантику в этот класс, чтобы он мог служить, как полноценный умный указатель.
    //ArrayPtr(ArrayPtr&&) = default;
    //ArrayPtr& operator=(ArrayPtr&&) = default;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    };

    ArrayPtr& operator=(ArrayPtr&& other) {
        if (this == &other)
            return *this;
        delete raw_ptr_;
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        return *this;
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* tmp = raw_ptr_;
        raw_ptr_ = nullptr;
        return tmp;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        return *(raw_ptr_ + index);
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        return *(raw_ptr_ + index);
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        // Заглушка. Реализуйте операцию самостоятельно
        return !raw_ptr_ == 0;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_,other.raw_ptr_);
       // Type* tmp = raw_ptr_;
       // raw_ptr_ = other.raw_ptr_;
       // other.raw_ptr_ = tmp;
    }

private:
    Type* raw_ptr_ = nullptr;
};
