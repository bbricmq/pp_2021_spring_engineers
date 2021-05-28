// Copyright 2021 Eremina Alena
#include "../../../modules/task_1/eremina_a_convex_hull_bin/convex_hull_bin.h"
#include <math.h>
#include <map>
#include <vector>
#include <stack>
#include <random>
#include <ctime>
#include <iostream>
#define PI 3.1416

// Generate binary image
std::vector<uint8_t> generateImage(const int wigth, const int height) {
    std::vector<uint8_t> img_src_dst(wigth*height);
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < wigth * height; i++) {
        int r = gen() % 5;
        if (r == 0) {
            img_src_dst[i] = 0;  // black
        } else {
            img_src_dst[i] = 255;  // white
        }
    }
    return img_src_dst;
}

// Split binary image into components
std::map<int, int> splitImageIntoComponents(std::vector<int>* img_src_dst,
    const int width, const int height) {
    int kn = 0, km = 0;
    int count = 1;
    int A, B, C;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            kn = j - 1;
            if (kn < 0) {
                kn = 0;
                B = 0;
            } else {
                B = img_src_dst->data()[i * width + kn];
            }
            km = i - 1;
            if (km < 0) {
                km = 0;
                C = 0;
            } else {
                C = img_src_dst->data()[km * width + j];
            }
            A = img_src_dst->data()[i * width + j];
            if (A == 0) {
                continue;
            } else if (B == 0 && C ==0) {
                count++;
                img_src_dst->data()[i * width + j] = count;
            } else if (B != 0 && C == 0) {
                img_src_dst->data()[i * width + j] = B;
            } else if (B == 0 && C != 0) {
                img_src_dst->data()[i * width + j] = C;
            } else if (B != 0 && C != 0) {
                if (B == C) {
                    img_src_dst->data()[i * width + j] = B;
                } else {
                    img_src_dst->data()[i * width + j] = B;
                    for (int u = 0; u < height; u++) {
                        for (int v = 0; v < width; v++) {
                            if (img_src_dst->data()[u * width + v] == C) {
                                img_src_dst->data()[u * width + v] = B;
                            }
                        }
                    }
                }
            }
        }
    }

    std::map<int, int> marks;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (img_src_dst->data()[i * width + j] == 0) {
                continue;
            } else {
                auto f = marks.find(img_src_dst->data()[i * width + j]);
                if (f != marks.end()) {
                    int count = f->second + 1;
                    marks[img_src_dst->data()[i * width + j]] = count;
                } else {
                    marks[img_src_dst->data()[i * width + j]] = 1;
                }
            }
        }
    }

    return marks;
}

// Sort points in ascending order of polar angle,
// measured counterclockwise from point p0;
// If the polar angles of several points coincide,
// then the distance to the point p0
void sortPolar(point* points, const point* p0, const int count) {
    for (int i = 0; i < count; i++) {
        int y = points[i].y - p0->y;
        int x = points[i].x - p0->x;
        double polar_angle = atan2(y, x);
        if (polar_angle < 0) polar_angle += 2*PI;
        points[i].polar_angle = polar_angle;
        points[i].distanse_p0 = sqrt(x * x + y * y);
    }

    point temp = {0, 0, 0, 0};
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (points[j].polar_angle > points[j + 1].polar_angle) {
                temp = points[j];
                points[j] = points[j + 1];
                points[j + 1] = temp;
            } else if (abs(points[j].polar_angle - points[j + 1].polar_angle)
                       < 0.000001) {
                if (points[j].distanse_p0 < points[j + 1].distanse_p0
                    && points[j].distanse_p0 != 0) {
                    temp = points[j];
                    points[j] = points[j + 1];
                    points[j + 1] = temp;
                }
            }
        }
    }
}

// Do the three points a, b and c make a right turn (with or without =)?
bool rightTurn(const point* a, const point* b, const point* c, bool flag) {
    point u = {b->x - a->x, b->y - a->y, 0, 0};
    point v = {c->x - b->x, c->y - b->y, 0, 0};
    if (flag) {
        return u.x * v.y - u.y * v.x <= 0;
    }
    return u.x * v.y - u.y * v.x < 0;
}

bool leftTurn(const point* a, const point* b, const point* c, bool flag) {
    point u = {b->x - a->x, b->y - a->y, 0, 0};
    point v = {c->x - b->x, c->y - b->y, 0, 0};
    if (flag) {
        return u.x * v.y - u.y * v.x >= 0;
    }
    return u.x * v.y - u.y * v.x > 0;
}

// Graham's algorithm - an algorithm for constructing a convex hull
// in two-dimensional space
std::vector<uint8_t> Convex_Hull(std::vector<uint8_t> img_src,
    const int height, const int width) {
    // Output data
    std::vector<uint8_t> img_dst(height * width, 255);

    // Copy img_src to img_mark
    std::vector<int> img_mark(height * width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (img_src[i * width + j] == 0) {
                img_mark[i * width + j] = 1;
            } else {
                img_mark[i * width + j] = 0;
            }
        }
    }

    // Split binary image into components
    std::map<int, int> marks = splitImageIntoComponents(&img_mark,
        width, height);

    for (auto mark : marks) {
        if (mark.second <= 3) {
            int cnt = 0;
            for (int i = 0; i < height && cnt < mark.second; i++) {
                for (int j = 0; j < width && cnt < mark.second; j++) {
                    if (img_mark[i * width + j] == mark.first) {
                        img_dst[i * width + j] = 0;
                        cnt++;
                    }
                }
            }
            continue;
        }
        // p0 - point with the minimum Y or the leftmost one
        point p0 = {width, height, 0, 0};
        // points - array of all points of the component
        point* points = new point[mark.second];
        // count - number of founded points of the component
        int count = 0;

        // find p0
        for (int i = 0; i < height && count < mark.second; i++) {
            for (int j = 0; j < width && count < mark.second; j++) {
                if (img_mark[i * width + j] == mark.first) {
                    points[count] = {j, i, 0, 0};
                    count++;
                    if (i < p0.y) {
                        p0.x = j;
                        p0.y = i;
                    }
                    if (i == p0.y) {
                        if (j < p0.x) {
                            p0.x = j;
                            p0.y = i;
                        }
                    }
                }
            }
        }

        // Sort points
        sortPolar(points, &p0, count);

        std::stack<point> S;
        S.push(p0);
        S.push(points[1]);
        for (int i = 2; i < count; i++) {
            point top = S.top();
            S.pop();
            point next_to_top = S.top();
            S.push(top);
            while (rightTurn(&next_to_top, &top, &(points[i]), 0)) {
                S.pop();
                top = S.top();
                S.pop();
                next_to_top = S.top();
                S.push(top);
            }
            S.push(points[i]);
        }

        point elem = {-1, -1, 0, 0};
        point begin = S.top();
        point b = begin;
        img_dst[begin.y * width + begin.x] = 0;
        S.pop();
        bool flag = true;
        while (flag) {
            if (!S.empty()) {
                elem = S.top();
                S.pop();
            } else {
                elem = b;
                flag = false;
            }
            if (abs(begin.x - elem.x) > 1 || abs(begin.y - elem.y) > 1) {
                if (begin.x == elem.x) {
                    int y = std::min(begin.y, elem.y) + 1;
                    for (; y < std::max(begin.y, elem.y); y++) {
                        img_dst[y * width + begin.x] = 0;
                    }
                } else {
                    double k = static_cast<double>(elem.y - begin.y) /
                               static_cast<double>(elem.x - begin.x);
                    double b = static_cast<double>(elem.x*begin.y
                      - begin.x * elem.y)
                      / static_cast<double>(elem.x - begin.x);
                    if (std::abs(begin.x - elem.x)
                     >= std::abs(begin.y - elem.y)) {
                        int x = std::min(begin.x, elem.x) + 1;
                        for (; x < std::max(begin.x, elem.x); x++) {
                            int y = static_cast<int>(std::round(k * x + b));
                            img_dst[y * width + x] = 0;
                        }
                    } else {
                        int y = std::min(begin.y, elem.y) + 1;
                        for (; y < std::max(begin.y, elem.y); y++) {
                            int x = static_cast<int>(std::round((y - b) / k));
                            img_dst[y * width + x] = 0;
                        }
                    }
                }
            }
            img_dst[elem.y * width + elem.x] = 0;
            begin = elem;
        }
        delete [] points;
    }
    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         if (img_dst[i * width + j] == 0) std::cout << "+";
    //         else std::cout << "_";
    //     }
    //     std::cout << std::endl;
    // }
    return img_dst;
}
