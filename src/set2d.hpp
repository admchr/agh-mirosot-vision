#ifndef SET_H
#define SET_H 

#include <vector>

template <typename T>
class Set2d {
public:
    Set2d(int width, int height);
    
    void set(int x, int y, T t);
    T get(int x, int y);
    
    template <typename F>
    void forEach(F f);
private:
    std::vector<T> vec;
    int width;
    int height;
    int getPos(int x, int y);
};


template <typename T>
int Set2d<T>::getPos(int x, int y) {
    if (x<0 || y<0)
        return -1;
    if (x>=width || y>= height)
        return -1;
    return width*y+x;
}

template <typename T>
Set2d<T>::Set2d(int width, int height) {
    vec.resize(width*height);
    this->width = width;
    this->height = height;
}

template <typename T>
void Set2d<T>::set(int x, int y, T t) {
    int pos = getPos(x, y);
    if (pos == -1) return;
    vec[pos]=t;
}
template <typename T>
T Set2d<T>::get(int x, int y) {
    int pos = getPos(x, y);
    return vec[pos];
}


#endif
