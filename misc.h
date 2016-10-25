#ifndef _MISC_H
#define _MISC_H

// ***************************************
// Call on scope exit (like finally)
// Example:
//   FILE * fp = fopen("test.out", "wb");
//   SCOPE_EXIT(fclose(fp));

template <typename F>
struct ScopeExit {
    ScopeExit(F f) : f(f) {}
    ~ScopeExit() { f(); }
    F f;
};

template <typename F>
ScopeExit<F> MakeScopeExit(F f) {
    return ScopeExit<F>(f);
};

#define SCOPE_EXIT(code)                                                \
    auto STRING_JOIN2(scope_exit_, __LINE__) = MakeScopeExit([=](){code;})

// ***************************************

template <class T> using remove_const_t = typename remove_const<T>::type;
template <class T> using remove_reference_t = typename remove_reference<T>::type;
template <class T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <class C> auto cbegin(const C& container)->decltype(std::begin(container)) {
    return std::begin(container);
}
template <class C> auto cend(const C& container)->decltype(std::end(container)) {
    return std::end(container);
}

// ***************************************
// Debug output
template<typename ... Types> void debug(Types ... args);
template<typename Type1, typename ... Types> void debug(Type1 arg1, Types ... rest) {
    std::cout << typeid(arg1).name() << ": " << arg1 << std::endl;
    debug(rest ...);
}
template<> void debug() {}
// ***************************************


template <typename T>
void f_impl(T val, true_type);
// for integer T
template <typename T>
void f_impl(T val, false_type);
// for others
template <typename T>
void f(T val)
{
    f_impl(val,std::is_integral<T>());
}

template <typename ... Args> class ContainerBundle {
public:
    using iterator = IteratorBundle<typename Args::iterator ...>;
    using iter_coll = std::tuple<typename Args::iterator ...>;
    ContainerBundle(typename std::add_pointer<Args>::type ... args)
        : dat{args ...}, bg{args->begin() ...}, nd{args->end() ...} {}
    ~ContainerBundle() = default;
    ContainerBundle(const ContainerBundle &) = delete;
    ContainerBundle(ContainerBundle &&) = default;
    inline iterator begin() { return bg; }
    inline iterator end() const { return nd; }
private:
    std::tuple<typename std::add_pointer<Args>::type ...> dat;
    iterator bg,nd;
};

template <typename ... Itr> class IteratorBundle {
public:
    using value_type = std::tuple<typename Itr::value_type ...>;
    using internal_type = std::tuple<Itr ...>;
    IteratorBundle() = default; // etc.
    bool operator==(const IteratorBundle &it) const { return loc==it.loc; }
    // special implementation insisting that all
    // elements in the bundle compare unequal. This ensures proper
    // function for containers of different sizes.
    bool operator!=(const IteratorBundle &it) const;
    inline value_type operator * () { return deref(); } inline IteratorBundle & operator++() {
        advance_them<0,sizeof ...(Itr)>::eval(loc);
        return
            }
    template <bool... b> struct static_all_of;
    template <bool... tail>
    struct static_all_of<true, tail...> : static_all_of<tail...> {};
    //no need to look further if first argument is false
    template <bool... tail>
    struct static_all_of<false, tail...> : std::false_type {};
    template <> struct static_all_of<> : std::true_type {};
    template <typename ... Args>
    ContainerBundle<typename std::remove_pointer<Args>::type ...> zip(Args ... args)
    {
        static_assert(static_all_of<std::is_pointer<Args>::value ...>::value,
                      "Each argument to zip must be a pointer to a container! ");
        return {args ...};
    }
};

#endif
