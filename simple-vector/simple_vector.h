#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <vector>

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
    explicit SimpleVector(size_t size) 
    :size_(size),
        capacity_(size),
        items_(size)
    {
            std::generate(begin(), end(), []() {return Type{}; });
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : SimpleVector(size)
    {
            std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
        : SimpleVector(init.size())
    {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    SimpleVector(const SimpleVector& other) 
        : SimpleVector(other.size_)
    {
        std::copy(other.begin(), other.end(), items_.Get());
    }

    SimpleVector(SimpleVector&& other)
    {
        swap(other);
    }

    SimpleVector(ReserveProxyObj capacity_to_reserve) {
        Reserve(capacity_to_reserve.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        //SimpleVector* hs = &rhs;
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) { throw std::out_of_range("out of range"); }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) { throw std::out_of_range("out of range"); }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        //Как и std::vector, класс SimpleVector может изменять свой размер в сторону увеличения и уменьшения.Для этого служит метод Resize :
        if (capacity_ == size_) {
            size_t new_capacity = 1;
            Reserve(std::max(new_capacity, capacity_ * 2));
         //   size_t new_capacity = capacity_;
         //   if (new_capacity <= 1) { new_capacity = capacity_ + 1; }
         //   else {
         //       new_capacity = capacity_ * 2;
         //   }
        //    Reserve(new_capacity);
        }
        items_[size_] = item;
        ++size_;
    }

    void PushBack(Type&& item) {
        //Как и std::vector, класс SimpleVector может изменять свой размер в сторону увеличения и уменьшения.Для этого служит метод Resize :
        if (capacity_ == size_) {
            size_t new_capacity = 1;
            Reserve(std::max(new_capacity, capacity_ * 2));
          //  size_t new_capacity = capacity_;
          //  if (new_capacity <= 1) { new_capacity = capacity_ + 1; }
          //  else {
          //      new_capacity = capacity_ * 2;
          //  }
         //   Reserve( new_capacity);
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(!IsEmpty());
         --size_;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        items_.swap(other.items_);
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size >= capacity_) {
            size_t new_capacity = capacity_;
            if (new_size <= 1) { new_capacity = capacity_+1; }
            else {
                new_capacity = std::max(new_size, capacity_ * 2);
            }
            ArrayPtr<Type> tmp(new_capacity);
            std::move(items_.Get(), items_.Get()+size_, tmp.Get());
            std::generate(tmp.Get() + size_, tmp.Get() + new_capacity, []() {return Type{}; });
            items_.swap(tmp);
            size_ = new_size;
            capacity_ = new_capacity;
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

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp(new_capacity);
            std::move(items_.Get(), items_.Get() + size_, tmp.Get());
            items_.swap(tmp);
            capacity_ = new_capacity;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos>=begin() && pos <= end());
        auto dist = pos - begin();
        if (capacity_ == size_) {
            size_t new_capacity = capacity_;
            if (new_capacity <= 1) { new_capacity = capacity_ + 1; }
            else {
                new_capacity = capacity_ * 2;
            }
            Reserve(new_capacity);
        }
        std::move_backward(cbegin()+dist, cend(), end() + 1);
        items_[dist] = value;
        ++size_;
        return begin()+dist;
    }

    Iterator Insert(ConstIterator pos,Type&& value) {
        //Как и std::vector, класс SimpleVector может изменять свой размер в сторону увеличения и уменьшения.Для этого служит метод Resize :
        assert(pos>= begin() && pos <= end());
        auto dist = pos - begin();
        if (capacity_ == size_) {
            size_t new_capacity = capacity_;
            if (new_capacity <= 1) { new_capacity = capacity_ + 1; }
            else {
                new_capacity = capacity_ * 2;
            }
            Reserve(new_capacity);
        }
        std::move_backward(begin() + dist, end(), end() + 1);
        items_[dist] = std::move(value);
        ++size_;
        return begin() + dist;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos>= begin() && pos < end());
        int64_t dist = std::distance(cbegin(), pos) ;
        std::move(begin()+dist + 1, end(), begin()+dist);
        --size_;
        return begin() + dist;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }
    private:
        ArrayPtr<Type> items_;
        size_t size_ = 0;
        size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}