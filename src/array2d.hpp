#ifndef SET_H
#define SET_H 

#include <vector>
#include <opencv/cv.h>

template <typename T>
class Array2d {
public:
    Array2d();
    Array2d(int width, int height);
    
    void resize(int x, int y);
    void resize(cv::Size size);
    void set(int x, int y, T t);
    T get(int x, int y);
    
    template <typename F>
    void forEach(F& f)  {
        for (int y=0;y<height;y++)
            for (int x=0;x<width;x++)
                vec[getPos(x, y)] = f(x, y, vec[getPos(x, y)]);
    }

private:
    std::vector<T> vec;
    int width;
    int height;
    int getPos(int x, int y);
};


template <typename T>
int Array2d<T>::getPos(int x, int y) {
    if (x<0 || y<0)
        return -1;
    if (x>=width || y>= height)
        return -1;
    return width*y+x;
}

template <typename T>
Array2d<T>::Array2d(int width, int height) {
    resize(width, height);
}
template <typename T>
Array2d<T>::Array2d() {
}

template <typename T>
void Array2d<T>::resize(int width, int height) {
    vec.resize(width*height);
    this->width = width;
    this->height = height;
}

template <typename T>
void Array2d<T>::resize(cv::Size size) {
    resize(size.width, size.height);
}

template <typename T>
void Array2d<T>::set(int x, int y, T t) {
    int pos = getPos(x, y);
    if (pos == -1) return;
    vec[pos]=t;
}
template <typename T>
T Array2d<T>::get(int x, int y) {
    int pos = getPos(x, y);
    return vec[pos];
}
#endif
