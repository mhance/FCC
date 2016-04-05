#include "make_limit_plots.C"
#include "make_contour_plots.C"

void make_all_plots(){
  
  make_all_contours("pdf");
  make_all_summaries();
}


