#include <stdio.h>
struct Date{
    int day;
    int month;
    int year;
};
int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
int getDaysInMonth(int month, int year) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    return (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}
long totalDays(struct Date date) {
    long total = 0;
    int y;
    for ( y = 1; y < date.year; y++) {
        total += isLeapYear(y) ? 366 : 365;
    }
    for (int m = 1; m < date.month; m++) {
        total += getDaysInMonth(m, date.year);
    }
    total += date.day;

    return total;
}
long daysBetween(struct Date date1, struct Date date2) {
    return totalDays(date2) - totalDays(date1);
}