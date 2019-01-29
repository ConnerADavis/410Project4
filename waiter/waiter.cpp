#include <string>
#include "stdlib.h"

#include "../includes/waiter.h"
#include "../includes/externs.h"
#include "../includes/PRINT.h"

using namespace std;
//Author: Conner Davis
Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder){
	return myIO.getNext(anOrder);
}

void Waiter::beWaiter() {
	b_WaiterIsFinished = false;
	while(true)
	{
		unique_lock<mutex> lck(mutex_order_inQ);
		ORDER anOrder;
		int lastRequest = getNext(anOrder);
		if(lastRequest == SUCCESS)
		{
			PRINT5("Waiter number ", id, " adding order number ", anOrder.order_number, " to order in queue");
			order_in_Q.push(anOrder);
			cv_order_inQ.notify_all();
		}
		else
		{
			PRINT1("Out of orders");
			break;
		}
	}

	// letting bakers know that waiter is done
	{
		unique_lock<mutex> lck(mutex_order_inQ);
		b_WaiterIsFinished = true;
		PRINT3("Waiter ", id, " clocking out")
	}

	cv_order_inQ.notify_all();
}

