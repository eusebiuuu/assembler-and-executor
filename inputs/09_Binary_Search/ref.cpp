int binsearch(const int arr[], int needle, int size) {
    int mid;
    int left = 0;
    int right = size -1;

    while (left <= right) {
        mid = (left + right) / 2;
        // Needle is bigger than what we're looking at,
        // only consider the upper half of the list.
        if (needle > arr[mid]) {
            left = mid + 1;
        }
        else if (needle < arr[mid]) {
            right = mid - 1;
        }
        else {
            return mid;
        }
    }
    // We've gone through all elements and the needle wasn't found.
    return -1;
}