struct Coordinate {
    double x;
    double y;
};
double distance(const Coordinate &from, const Coordinate &to) {
    double x = to.x - from.x;
    double y = to.y - from.y;
    return sqrt(x*x + y*y);
}