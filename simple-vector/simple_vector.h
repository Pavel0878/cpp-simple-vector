#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>

#include "array_ptr.h"


struct ReserveProxyObj {
    ReserveProxyObj(size_t capacity_to_reserve)
        :capacity_(capacity_to_reserve) {};
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
            ArrayPtr<Type> tmp(size);
            items_.swap(tmp);
            size_ = size;
            capacity_ = size;
            std::generate(begin(), end(), []() {return Type{}; });
        // Напишите тело конструктора самостоятельно
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
            ArrayPtr<Type> tmp(size);
            items_.swap(tmp);
            size_ = size;
            capacity_ = size;
            std::fill(begin(), end(), value);
        // Напишите тело конструктора самостоятельно
    }

    SimpleVector(size_t size, Type&& value) {
        ArrayPtr<Type> tmp(size);
        items_.swap(tmp);
        size_ = size;
        capacity_ = size;
        std::generate(begin(), end(), value);
        // Напишите тело конструктора самостоятельно
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        ArrayPtr<Type> tmp(init.size());
        items_.swap(tmp);
        size_ = init.size();
        capacity_ = init.size();
        std::move(init.begin(), init.end(), items_.Get());
        // Напишите тело конструктора самостоятельно
    }

    SimpleVector(const SimpleVector& other)  {
        ArrayPtr<Type> tmp(other.size_);
        items_.swap(tmp);
        size_ = other.size_;
        capacity_ = other.size_;
        std::generate(begin(), end(), []() {return Type{}; });
        std::move(other.begin(), other.end(), items_.Get());
        // Напишите тело конструктора самостоятельно
    }

    SimpleVector(SimpleVector&& other) {
        ArrayPtr<Type> tmp(other.size_);
        items_.swap(tmp);
        size_ = other.size_;
        capacity_ = other.size_;
        std::generate(begin(), end(), []() {return Type{}; });
        std::move(other.begin(), other.end(), items_.Get());
        std::exchange(other.size_, 0);
        // Напишите тело конструктора самостоятельно
    }

    SimpleVector(ReserveProxyObj capacity_to_reserve) {
        capacity_ = capacity_to_reserve.capacity_;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
        // Напишите тело самостоятельно
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
        // Напишите тело самостоятельно
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        SimpleVector tmp(rhs);
        swap(tmp);
        // Напишите тело конструктора самостоятельно
        return *this;
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) { throw std::out_of_range("out of range"); }
        return items_[index];
        // Напишите тело самостоятельно
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) { throw std::out_of_range("out of range"); }
        return items_[index];
        // Напишите тело самостоятельно
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
        // Напишите тело самостоятельно
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (capacity_ == size_) { Resize(size_); }
        items_[size_] = item;
        ++size_;
        // Напишите тело самостоятельно
    }

    void PushBack(Type&& item) {
        if (capacity_ == size_) { Resize(size_); }
        items_[size_] = std::move(item);
        ++size_;
        // Напишите тело самостоятельно
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (!IsEmpty()) { --size_; }
        // Напишите тело самостоятельно
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        size_t tmp = size_;
        size_t tmp1 = capacity_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.size_ = tmp;
        other.capacity_ = tmp1;
        // Напишите тело самостоятельно
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size >= capacity_) {
            size_t new_capacity_ = capacity_;
            if (new_size <= 1) { new_capacity_ = capacity_+1; }
            else {
                new_capacity_ = std::max(new_size, capacity_ * 2);
            }
            ArrayPtr<Type> tmp(new_capacity_);
            std::move(items_.Get(), items_.Get()+size_, tmp.Get());
            std::generate(tmp.Get() + size_, tmp.Get() + new_capacity_, []() {return Type{}; });
            items_.swap(tmp);
            size_ = new_size;
            capacity_ = new_capacity_;
        }
        else {
            if (new_size > size_) {
                std::generate(items_.Get() + size_, items_.Get() + new_size, []() {return Type{}; });
                size_ = new_size;
            }
            else { size_ = new_size; }
        }
        // Напишите тело самостоятельно
    }

    void Reserve(size_t new_capacity_) {
        if (new_capacity_ > capacity_) {
            ArrayPtr<Type> tmp(new_capacity_);
            std::move(items_.Get(), items_.Get() + size_, tmp.Get());
            std::generate(tmp.Get() + size_, tmp.Get() + new_capacity_, []() {return Type{}; });
            items_.swap(tmp);
            capacity_ = new_capacity_;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        auto dist = pos - begin();
        if (capacity_ == size_) { Resize(size_); }
        std::move_backward(cbegin()+dist, cend(), end() + 1);
        items_[dist] = value;
        ++size_;
        return begin()+dist;
        // Напишите тело самостоятельно
    }

    Iterator Insert(ConstIterator pos,Type&& value) {
        auto dist = pos - begin();
        if (capacity_ == size_) { Resize(size_); }
        std::move_backward(begin() + dist, end(), end() + 1);
        items_[dist] = std::move(value);
        ++size_;
        return begin() + dist;
        // Напишите тело самостоятельно
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        int64_t dist = std::distance(cbegin(), pos) ;
        std::move(begin()+dist + 1, end(), begin()+dist);
        --size_;
        
        return begin() + dist;
        // Напишите тело самостоятельно
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
        // Напишите тело самостоятельно
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
        // Напишите тело самостоятельно
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
        // Напишите тело самостоятельно
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
        // Напишите тело самостоятельно
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
        // Напишите тело самостоятельно
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
        // Напишите тело самостоятельно
    }
    private:
        ArrayPtr<Type> items_;
        size_t size_ = 0;
        size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
   // return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs < rhs);
}