#ifndef UTILS_H
#define UTILS_H

#define F_PTR(class, name, ...) static_cast<void (class::*)(__VA_ARGS__)>(&class::name)

struct Scope {
    std::function<void()> on_exit;
    ~Scope() { if(on_exit) on_exit(); }
};

#endif // UTILS_H
