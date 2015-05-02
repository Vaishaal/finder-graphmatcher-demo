/**
 *  shadow_util.cpp
 *
 *  definitions of range class and mathematical heavy-lifters
 */

#include <cmath>
#include <cstdlib>

#include "shadow_util.h"

using namespace std;

range::range() {
   values = new vector<float>(0);
}

range::range(float lo, float hi, unsigned count) {
   values = new vector<float>(count);
   my_low = lo;
   my_high = hi;
   for(index = 0; index < count; index++) {
      (*values)[index] = lo + (index*(hi - lo)/(count - 1));
   }
}

range::range(const vector<float> & v) {
   values = new vector<float>(v.size());
   if(values->size() > 0) {
      my_low = my_high = (*values)[0] = v[0];
      for(index = 1; index < values->size(); index++) {
         (*values)[index] = v[index];
         if(v[index] < my_low) { my_low = v[index]; }
         if(v[index] > my_high) { my_high = v[index]; }
      }
   }
}

range::range(const range & other) {
   this->values = new vector<float>(*(other.values));
   this->my_low = other.my_low;
   this->my_high = other.my_high;
   this->index = this->values->size();
}

range range::operator=(const range & other) {
   this->values->resize(other.values->size());
   if(other.empty()) { return *this; }
   for(this->index = 0;
         this->index < other.values->size();
         this->index++) {
      (*(this->values))[this->index] = (*(other.values))[this->index];
   }
   this->my_low = other.my_low;
   this->my_high = other.my_high;
   return *this;
}

range::~range() {
   delete(values);
}

float range::high() const { return my_high; }
float range::low() const { return my_low; }
float range::card() const { return values->size(); }

void range::init() { index = 0; return; }
bool range::has_next() { return index < values->size(); }
float range::next() {
   float temp = (*values)[index];
   index++;
   return temp;
}

bool range::empty() const { return (values->size() == 0); }
bool range::singular() const { return (values->size() == 1); }
bool range::contains(float f) const {
   return (!empty() && (my_low <= f) && (f <= my_high));
}

void range::add(float f) {
   values->push_back(f);
   if(f > my_high || values->size() == 1) { my_high = f; }
   if(f < my_low || values->size() == 1) { my_low = f; }
   index = values->size();
}

void range::sort() {
   sort(0, values->size());
   index = values->size();
}

void range::intersect(range & other) {
   if(other.empty()) { return; }
   if(this->empty()) { other.values->resize(0); return; }
   if(this->my_low > other.my_low) { other.my_low = this->my_low; }
   if(this->my_high < other.my_high) {other.my_high = this->my_high; }
   vector<float> temp(*(other.values));
   other.values->resize(0);
   for(other.index = 0; other.index < temp.size(); other.index++) {
      if(other.my_low <= temp[other.index]
            && temp[other.index] <= other.my_high) {
         other.values->push_back(temp[other.index]);
      }
   }
   for(other.index = 0;
         other.index < this->values->size();
         other.index++) {
      if(other.my_low <= (*(this->values))[other.index]
            && (*(this->values))[other.index] <= other.my_high) {
         other.values->push_back((*(this->values))[other.index]);
      }
   }
   other.index = other.values->size();
   return;
}

void range::unite(vector<range*> & others) {
   if(this->empty()) { return; }
   others.push_back(new range(*this));
   sort_range_vector(others);
   unsigned i, j;
   /* remove empty ranges */
   for(i = 0; i < others.size(); i++) {
      while(others[i]->empty()) {
         delete others[i];
         for(j = i+1; j < others.size(); j++) {
            others[j-1] = others[j];
         }
         others.resize(j-1);
      }
   }
   /* unite overlapping ranges */
   for(i = 0; i < others.size()-1; i++) {
      if(others[i]->my_high >= others[i+1]->my_low) {
         if(others[i]->my_high > others[i+1]->my_high) {
            others[i]->my_high = others[i+1]->my_high;
         }
         for(j = 0; j < others[i+1]->values->size(); j++) {
            others[i]->values->push_back((*(others[i+1]->values))[j]);
         }
         others[i]->index = others[i]->values->size();
         delete others[i+1];
         for(j = i+2; j < others.size(); j++) {
            others[j-1] = others[j];
         }
         others.resize(j-1);
         /* repeat this range on next iteration, *
          *  because it may ovelap more          */
         i--;
      }
   }
   return;
}

/* quicksort */
void range::sort(unsigned start, unsigned end) {
   if(end <= start + 1) { return; }
   float temp, pval;
   unsigned i, j, pivot;
   pivot = rand()%(end - start) + start;
   pval = (*values)[pivot];
   (*values)[pivot] = (*values)[end - 1];
   (*values)[end - 1] = pval;
   i = start, j = end - 1;
   while(i < j) {
      while(i < j && (*values)[i] < pval) { i++; }
      while(i < j && (*values)[j] >= pval) { j--; }
      if(i < j) {
         temp = (*values)[i];
         (*values)[i] = (*values)[j];
         (*values)[j] = temp;
      }
   }
   (*values)[end - 1] = (*values)[i];
   (*values)[i] = pval;
   sort(start, i);
   sort(i + 1, end);
   return;
}

/* insertion sort, rather than an nlogn sort, because *
 *  common case is only one element is out of place   */
void sort_range_vector(vector<range*> & to_sort) {
   unsigned sorted, slot, index;
   range* temp;
   for(sorted = 1; sorted < to_sort.size(); sorted++) {
      if(to_sort[sorted]->low() >= to_sort[sorted-1]->low()) {
         continue;
      }
      /* to_sort[sorted] belongs somewhere earlier */
      temp = to_sort[sorted];
      for(slot = 0;
            slot < sorted && to_sort[slot]->low() < temp->low();
            slot++);
      for(index = sorted; index > slot; index--) {
         to_sort[index] = to_sort[index-1];
      }
      to_sort[slot] = temp;
   }
   return;
}

float angular_hypotenuse(float a, float b) {
   const float pi = 3.14159265f;
   if(a >= pi || b >= pi) { return pi; }
   float t2a, t2b;
   t2a = pow(tan(a), 2);
   t2b = pow(tan(b), 2);
   return atan(sqrt(t2a + t2b + t2a*t2b));
}

float angular_pythagorus(float c, float a) {
   const float pi = 3.14159265f;
   if(c >= pi) { return pi; }
   float t2a, t2c;
   t2a = pow(tan(a), 2);
   t2c = pow(tan(c), 2);
   return atan(sqrt((t2c - t2a)/(1 + t2a)));
}

void rotate(const vector<float> & rotation,
               const vector<float> & point,
               vector<float> & rotated_point) {
   rotated_point.resize(3);
   unsigned i, j;
   for(i = 0; i < 3; i++) {
      rotated_point[i] = 0;
      for(j = 0; j < 3; j++) {
         rotated_point[i] += rotation[3*i+j]*point[j];
      }
   }
   return;
}

float angular_cos(float c, float a) {
   return atan(tan(c)*cos(a));
}

float angular_sin(float c, float a) {
   return atan(tan(c)*sin(a));
}
