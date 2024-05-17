#include "triangle.h"
#include "display.h"

//TODO: Create implementation for triangle.h functions

void swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2) <---(Mx,My)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	//start from the top(x0,x0) calculate the slope1 of left edge and slope2 of right edge
	float inverse_slope_1 = (float)(x1 - x0) / (y1 - y0);
	float inverse_slope_2 = (float)(x2 - x0) / (y2 - y0);

	//initialize x_start and x_end from the top vertex (x0,y0)
	float x_start = x0;
	float x_end = x0;

	//Loop all scanlines from top to bottom (y0 to y2)
	for (int y = y0; y <= y2; y++)
	{
		//draw a line from x_start until x_end
		draw_line(round(x_start), y, round(x_end), y, color);

		//based on the slope value increment x_start and x_end for the next scanline
		x_start += inverse_slope_1;
		x_end += inverse_slope_2;
	}

}
///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//        (x0,y0)-----------(x1,y1)  <---(Mx,My)
//              \           /
//               \         /
//                \       /   
//                 \     /   
//                  \   /
//                   \ /            
//                  (x2,y2)
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	//start from the bottom (x2,x2) calculate the slope1 of left edge and slope2 of right edge
	float inverse_slope_1 = (float)(x2 - x0) / (y2 - y0);
	float inverse_slope_2 = (float)(x2 - x1) / (y2 - y1);

	//initialize x_start and x_end from the bottom vertex (x2,y2)
	float x_start = x2;
	float x_end = x2;

	//Loop all scanlines from bottom to top (y2 to y0)
	for (int y = y2; y >= y0; y--)
	{
		//draw a line from x_start until x_end
		draw_line(round(x_start), y, round(x_end), y, color);

		//based on the slope value increment x_start and x_end for the next scanline
		x_start -= inverse_slope_1;
		x_end -= inverse_slope_2;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
//////////////////////////////////////////////////////////////////////////////////
//
//                 (x0,y0)
//                   / \
//                  /   \
//                 /     \    
//                /       \
//               /         \
//         (x1,y1)---------(Mx,My)
//              \            \
//                --          \
//                   \         \
//                     --       \
//                        \      \
//                          --    \
//                             \   \
//                               -- \
//                                   \    
//                                 (x2,y2)

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	//we need to sort the vertices by y coordinates ascending(y0<y1<y2)
	if (y0 > y1){
		swap(&y0, &y1);
		swap(&x0, &x1);
	}
	if (y1 > y2){
		swap(&y1, &y2);
		swap(&x1, &x2);
	}
	if (y0 > y1){
		swap(&y0, &y1);
		swap(&x0, &x1);
	}

	if (y1 == y2)
	{
		// we can only draw flat-bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else if (y0 == y1)
	{	// we can only draw flat-top triangle
		fill_flat_top_triangle(x0, y0, x1,y1, x2, y2, color);
	}
	else
	{
		//calculate new vertex (Mx,My) using triangle similarity
		int My = y1;
		int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;


		//draw flat bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

		//draw flat top triangle
		fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);

	}
	


	

	


}