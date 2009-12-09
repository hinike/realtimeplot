#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
	config.min_x = -10;
	config.max_x = 90;
	config.min_y = -20;
	config.max_y = 30;

	Plot pl = Plot( config );
	for (int i=0; i<50;++i) {
		pl.line_add( (i-1)*10, (i-4)*5  );
		sleep(1);
	}
	pl.line_add( 10, 10 );
	return 0;
}