// Copyright 2021 Chesnokov Artyom

#include <cmath>
#include <climits>
#include <limits>
#include "../../../modules/task_3/chesnokov_a_jarvis/jarvis.h"

bool almost_equal(double x, double y, int ulp) {
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::fabs(x - y) <= std::numeric_limits<double>::epsilon() * std::fabs(x + y) * ulp
    // unless the result is subnormal
    || std::fabs(x - y) < std::numeric_limits<double>::min();
}

std::vector<Point> Jarvis::makeHull(const std::list<Point> & in) {
  if (in.empty()) {
    // empty hull for empty list
    return std::vector<Point>();
  }

  // resulting convex hull
  std::vector<Point> hull;

  // for one point P the CH is the P
  if (in.size() == 1) {
    hull.emplace_back(in.front());
    return hull;
  }

  // 1) find the leftmost Point p0, it will be first CH's vertex
  hull.emplace_back(findLeftmost(in));

  // 2) find the second vertex of CH, assuming that first one is the leftmost
  // imag - is imaginary previous vertex, that is the same as p0 but shifted down
  Point imag = LAST(hull);
  imag.y -= 30.0;
  hull.emplace_back(findWithMinAngle(imag, LAST(hull), &in));

  // 3) find next point of hull while last found != p0
  while (LAST(hull) != hull[0]) {
    hull.emplace_back(findWithMinAngle(PRE_LAST(hull), LAST(hull), &in));
  }

  hull.pop_back();

  return hull;
}

std::vector<Point> Jarvis::makeHullTbb(const std::list<Point>& in) {
  if (in.empty()) {
    // empty hull for empty list
    return std::vector<Point>();
  }

  // resulting convex hull
  std::vector<Point> hull;

  // for one point P the CH is the P
  if (in.size() == 1) {
    hull.emplace_back(in.front());
    return hull;
  }

  if (in.size() == 2) {
    hull.emplace_back(in.back());
    hull.emplace_back(in.front());
    return hull;
  }

  int turn = 0;

  tbb::task_scheduler_init init(4);
  // tbb::task_scheduler_init init();  // auto-detection
  tbb::parallel_for(tbb::blocked_range<int>(0, 4, 1), JarvisHull(in, &hull, &turn, 4));

  return hull;
}

void Jarvis::makeHullTbbSubRoutine(const std::list<Point>& in, std::vector<Point>* out,
  int* turn, int sector_num, int div) {
  static const double Pi = 3.14159265358979;

  // divide the plane into sectors of responsibility
  double sector = 2 * Pi / div;
  // find start and end vectors
  Point startvec(cos(Pi - sector_num * sector), sin(Pi - sector_num * sector));
  Point endvec(cos(Pi - (sector_num + 1) * sector), sin(Pi - (sector_num + 1) * sector));

  // find start and end points
  Point start = findWithMaxProjection(startvec, in);
  Point end = findWithMaxProjection(endvec, in);

  std::vector<Point> subHull;
  if (start != end || div == 1) {
    // start will be the first point in subHull
    subHull.emplace_back(start);
    // find imaginary point
    Point imag = start + Point(-startvec.y, startvec.x);
    subHull.emplace_back(findWithMinAngle(imag, LAST(subHull), &in));
    // find next point of hull while last found != end
    while (LAST(subHull) != end) {
      subHull.emplace_back(findWithMinAngle(PRE_LAST(subHull), LAST(subHull), &in));
    }

    subHull.pop_back();
  }

  while (*turn != sector_num) {
    std::cout << "";
  }

  (*out).insert((*out).end(), subHull.begin(), subHull.end());
  (*turn)++;
}

Point Jarvis::findLeftmost(const std::list<Point>& in) {
  // leftmost
  auto lm = in.begin();

  for (auto it = in.begin(); it != in.end(); it++) {
    if (almost_equal(it->x, lm->x, 2) && it->y < lm->y) {
      lm = it;
    } else if (it->x < lm->x) {
      lm = it;
    }
  }

  // it's save to return list's iterator (no invalidation)
  return *lm;
}

Point Jarvis::findWithMinAngle(const Point& prev, const Point& cur, const std::list<Point>* in) {
  Point vec1 = cur - prev;
  // Stores the required point
  std::list<Point>::const_iterator min_point_it = in->begin();
  // The smaller angle between two vectors the bigger cos
  double max_cos = -2.0;
  for (auto it = in->begin(); it != in->end(); it++) {
    Point vec2 = *it - cur;
    double cur_cos = vec1.cosine(vec2);

    if (*it == cur) continue;

    if (almost_equal(cur_cos, max_cos, 2) && cur.distance(*min_point_it) < cur.distance(*it)) {
      max_cos = cur_cos;
      min_point_it = it;
    } else if (cur_cos > max_cos) {
      max_cos = cur_cos;
      min_point_it = it;
    }
  }

  Point res = *min_point_it;
  // in->erase(min_point_it);

  return res;
}

Point Jarvis::findWithMaxProjection(const Point vec, const std::list<Point>& in) {
  double maxProj = 0;
  std::list<Point>::const_iterator answer = in.begin();
  for (auto it = in.begin(); it != in.end(); it++) {
    double proj = (it->x * vec.x + it->y * vec.y) / vec.length();
    if (proj > maxProj) {
      maxProj = proj;
      answer = it;
    }
  }
  return *answer;
}
