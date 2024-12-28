#include <iostream>
#include <map>
#include <memory>
#include <iterator>

template <typename T>
class CustomAllocator {
public:
    using value_type = T;

    explicit CustomAllocator(size_t capacity = 10) : capacity_(capacity), used_(0) {
        data_ = static_cast<T*>(::operator new(capacity_ * sizeof(T)));
    }

    template <typename U>
    CustomAllocator(const CustomAllocator<U>& other) noexcept
        : capacity_(other.capacity_), used_(0) {
        data_ = static_cast<T*>(::operator new(capacity_ * sizeof(T)));
    }

~CustomAllocator() {
    clear(); // Уничтожаем объекты
    // Здесь не нужно вызывать delete, потому что память уже освобождена в resize
}


    T* allocate(size_t n) {
        if (used_ + n > capacity_) {
            resize(std::max(capacity_ * 2, used_ + n));
        }
        T* result = data_ + used_;
        used_ += n;
        return result;
    }

void deallocate(T* p, size_t n) {
    if (p == nullptr) return;
    if (p < data_ || p >= data_ + capacity_) {
        throw std::runtime_error("Attempted to deallocate memory not owned by allocator");
    }
    // Память будет освобождена в деструкторе
}

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    template <typename U>
    struct rebind {
        using other = CustomAllocator<U>;
    };

    // Оператор сравнения для корректной работы std::map
    bool operator==(const CustomAllocator&) const noexcept { return true; }
    bool operator!=(const CustomAllocator&) const noexcept { return false; }

    size_t capacity() const { return capacity_; }

private:
    T* data_;
    size_t capacity_;
    size_t used_;

    void resize(size_t new_capacity) {
        if (new_capacity <= used_) {
            throw std::runtime_error("New capacity must be greater than the number of used elements");
        }
        T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
        std::uninitialized_move(data_, data_ + used_, new_data);
        clear();
        if (data_ != nullptr) {
            ::operator delete(data_); // Освобождаем старую память
        }
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void clear() {
        if (data_ != nullptr) {
            for (size_t i = 0; i < used_; ++i) {
                (data_ + i)->~T();
            }
            used_ = 0;
        }
    }

    template <typename U>
    friend class CustomAllocator;
};

template <typename T, typename Allocator = std::allocator<T>>
class CustomContainer {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using iterator = T*;
    using const_iterator = const T*;

    explicit CustomContainer(size_t initial_capacity = 10)
        : size_(0), capacity_(initial_capacity), data_(alloc_.allocate(initial_capacity)) {}

~CustomContainer() {
    clear();  // Уничтожаем объекты
    alloc_.deallocate(data_, capacity_);
}

    void push_back(const T& value) {
        if (size_ == capacity_) {
            resize(capacity_ * 2);
        }
        alloc_.construct(data_ + size_, value);
        ++size_;
    }

    iterator begin() { return data_; }
    iterator end() { return data_ + size_; }
    const_iterator begin() const { return data_; }
    const_iterator end() const { return data_ + size_; }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

private:
    Allocator alloc_;
    T* data_;
    size_t size_;
    size_t capacity_;

    void resize(size_t new_capacity) {
        T* new_data = alloc_.allocate(new_capacity);
        std::uninitialized_move(data_, data_ + size_, new_data);
        clear();
        alloc_.deallocate(data_, capacity_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            alloc_.destroy(data_ + i);
        }
        size_ = 0;
    }
};


int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}


int main() {
    // 1. std::map<int, int>
    std::cout << "std::map" << std::endl; 
    std::map<int, int> stdMap;
    for (int i = 0; i < 10; ++i) {
        stdMap[i] = factorial(i);
    }
    for (const auto& pair : stdMap) {
        std::cout << pair.first << " " << pair.second << "\n";
    }

    // 3. std::map<int, int> с пользовательским аллокатором
    std::cout << "std::map с пользовательским аллокатором" << std::endl; 
    std::map<int, int, std::less<int>, CustomAllocator<std::pair<const int, int>>>
        customMap(CustomAllocator<std::pair<const int, int>>(10));
    for (int i = 0; i < 10; ++i) {
        customMap[i] = factorial(i);
    }
    for (const auto& pair : customMap) {
        std::cout << pair.first << " " << pair.second << "\n";
    }

    // 6. Пользовательский контейнер
    std::cout << "Пользовательский контейнер" << std::endl; 
    CustomContainer<int> customContainer;
    for (int i = 0; i < 10; ++i) {
        customContainer.push_back(i);
    }
    for (const auto& value : customContainer) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    // 8. Пользовательский контейнер с пользовательским аллокатором
    std::cout << "Пользовательский контейнер с пользовательским аллокатором" << std::endl;
    CustomContainer<int, CustomAllocator<int>> customAllocContainer(10);
    for (int i = 0; i < 10; ++i) {
        customAllocContainer.push_back(i);
    }
    for (const auto& value : customAllocContainer) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    return 0;
}