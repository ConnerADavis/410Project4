#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
#include "../includes/PRINT.h"
#include "../includes/box.h"

using namespace std;
// Author: Conner Davis
Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

void Baker::bake_and_box(ORDER &anOrder) {
	int i = 0;
	// every time a box is filled but there's still donuts to be made
	// add another box
	while(i < anOrder.number_donuts)
	{
		Box aBox;
		// as long as the box's size is less than the max a box can hold
		// add donuts
		while(aBox.size() < 12)
		{
			// only add donuts if doing so wouldn't put you over the intended number of donuts
			if(i < anOrder.number_donuts)
			{
				DONUT donut;
				aBox.addDonut(donut);
				i++;
			}
			else
			{
				break;
			}
		}
		anOrder.boxes.push_back(aBox);
	}
	PRINT4("Baker number ", id, " has filled order number ", anOrder.order_number);
	mutex_order_outQ.lock();
	order_out_Vector.push_back(anOrder);
	mutex_order_outQ.unlock();
}

void Baker::beBaker() {
	while(true)
	{
		{
			unique_lock<mutex> lck(mutex_order_inQ);

			while(order_in_Q.empty() && !b_WaiterIsFinished)
			{
				cv_order_inQ.wait(lck);
			}
		}
		if(!order_in_Q.empty())
		{
			mutex_order_inQ.lock();
			ORDER order = order_in_Q.front();
			order_in_Q.pop();
			PRINT4("Baker number ", id, " baking order number ", order.order_number);
			mutex_order_inQ.unlock();
			bake_and_box(order);
		}

		//checking if it can stop
		if(order_in_Q.empty() && b_WaiterIsFinished)
		{
			break;
		}
	}
}
