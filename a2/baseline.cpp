//Ashley Horvat a1765128
//Eamon Fitzgerald
//Tom Zhu
//OS Group

/*
created by Andrey Kan
andrey.kan@adelaide.edu.au
2021
*/
#include <iostream>
#include <fstream>
#include <deque>
#include <vector>

// std is a namespace: https://www.cplusplus.com/doc/oldtutorial/namespaces/
const int TIME_ALLOWANCE = 8;  // allow to use up to this number of time slots at once
const int PRINT_LOG = 0; // print detailed execution trace

class Customer
{
public:
    std::string name;
    int priority;
    int arrival_time;
    int slots_remaining; // how many time slots are still needed
    int playing_since;

    Customer(std::string par_name, int par_priority, int par_arrival_time, int par_slots_remaining)
    {
        name = par_name;
        priority = par_priority;
        arrival_time = par_arrival_time;
        slots_remaining = par_slots_remaining;
        playing_since = -1;
    }
};

class Event
{
public:
    int event_time;
    int customer_id;  // each event involes exactly one customer

    Event(int par_event_time, int par_customer_id)
    {
        event_time = par_event_time;
        customer_id = par_customer_id;
    }
};

void initialize_system(
    std::ifstream &in_file,
    std::deque<Event> &arrival_events,
    std::vector<Customer> &customers)
{
    std::string name;
    int priority, arrival_time, slots_requested;

    // read input file line by line
    // https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c
    int customer_id = 0;
    while (in_file >> name >> priority >> arrival_time >> slots_requested)
    {
        Customer customer_from_file(name, priority, arrival_time, slots_requested);
        customers.push_back(customer_from_file);

        // new customer arrival event
        Event arrival_event(arrival_time, customer_id);
        arrival_events.push_back(arrival_event);

        customer_id++;
    }
}

void print_state(
    std::ofstream &out_file,
    int current_time,
    int current_id,
    const std::deque<Event> &arrival_events,
    const std::deque<int> &customer_queue)
{
    out_file << current_time << " " << current_id << '\n';
    if (PRINT_LOG == 0)
    {
        return;
    }
    std::cout << current_time << ", " << current_id << '\n';
    for (int i = 0; i < arrival_events.size(); i++)
    {
        std::cout << "\t" << arrival_events[i].event_time << ", " << arrival_events[i].customer_id << ", ";
    }
    std::cout << '\n';
    for (int i = 0; i < customer_queue.size(); i++)
    {
        std::cout << "\t" << customer_queue[i] << ", ";
    }
    std::cout << '\n';
}


// process command line arguments
// https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Provide input and output file names." << std::endl;
        return -1;
    }
    std::ifstream in_file(argv[1]);
    std::ofstream out_file(argv[2]);
    if ((!in_file) || (!out_file))
    {
        std::cerr << "Cannot open one of the files." << std::endl;
        return -1;
    }

    // deque: https://www.geeksforgeeks.org/deque-cpp-stl/
    // vector: https://www.geeksforgeeks.org/vector-in-cpp-stl/
    std::deque<Event> arrival_events; // new customer arrivals
    std::vector<Customer> customers; // information about each customer

    // read information from file, initialize events queue
    initialize_system(in_file, arrival_events, customers);

    int current_id = -1; // who is using the machine now, -1 means nobody
    int time_out = -1; // time when current customer will be preempted
    std::deque<int> queue; // waiting queue for regular customers
    std::deque<int> highPriority; // waiting queue for customers with a higher prioirty
    
   


    // step by step simulation of each time slot
    bool all_done = false;
    for (int current_time = 0; !all_done; current_time++)
    {
        // welcome newly arrived customers
        while (!arrival_events.empty() && (current_time == arrival_events[0].event_time))
        {
     
      
          queue.push_back(arrival_events[0].customer_id); //adding events to queue adding new customers straight to back

         
          arrival_events.pop_front();
          //sorting out queue first need to determine if there's already customers in the queue
          if(queue.size() > 0)
          {
          	int queue_front = queue.front();
          	int queue_back = queue.back();
          	//sort each customer in a priority queue based on their priority
          	if(customers[queue_back].priority == 0)
          	{
          		if(highPriority.size() > 0)
          		{
          	          int id = highPriority.front();
          			
          				
          				if(customers[queue_back].slots_remaining < customers[id].slots_remaining)
          				{        
          					highPriority.push_front(queue_back);
          					queue.pop_back();
          				

          				}
                        else
                        {
                            highPriority.push_back(queue_back);
                            queue.pop_back();
                        }
          			  
                }
            }
            else
            {
                if(customers[queue_back].slots_remaining < customers[queue_front].slots_remaining)
                {
                    queue.push_front(queue_back);
                    queue.pop_back();
                }
            }
       
          }
        }
       


        // check if we need to take a customer off the machine
        if (current_id >= 0)
        {
            if (current_time == time_out) // runtime has gone through 8 seconds or customer has finished
            {
                int last_run = current_time - customers[current_id].playing_since;
                customers[current_id].slots_remaining -= last_run;
                if (customers[current_id].slots_remaining > 0)
                {
                	//for a high priority customer
                	if(customers[current_id].priority = 0)
                	{
                		if(highPriority.size() > 0)
                		{
                			int id_priority = highPriority.front();
                			if(customers[current_id].slots_remaining < customers[id_priority].slots_remaining)
                    	{
                    		//if current_id smaller push to front of the queue to keep running
                    		highPriority.push_front(current_id);
                    		
                    	}
                        else
                        { // push current_id to back of queue
                          highPriority.push_back(current_id);
                        }

                		}
                	}

                	// need to check if there is a customer waiting in queue && for regualr customers
                    else if(queue.size() > 0)
                    {
                    	int next_id = queue.front();
                    	//check if the next customer has a smaller wait time then current customer
                    	if(customers[current_id].slots_remaining < customers[next_id].slots_remaining)
                    	{
                    		//if current_id smaller push to front of the queue to keep running
                    		queue.push_front(current_id);
                    		
                    	}
                        else
                        { // push current_id to back of queue
                          queue.push_back(current_id);
                        }
                     

                  
                    }
                    // else in case current_id is the only customer in current queue
                    else{
                    queue.push_back(current_id);
                      }
                    

                }
                current_id = -1; // the machine is free now
            }
        }
        // if machine is empty, schedule a new customer
        if (current_id == -1)
        {
        	//first check to see if priority queue is empty
        	if(!highPriority.empty())
        	{
        		current_id = highPriority.front();
                highPriority.pop_front();
                if (TIME_ALLOWANCE > customers[current_id].slots_remaining)
                {
                    time_out = current_time + customers[current_id].slots_remaining;
                }
                else
                {
                    time_out = current_time + TIME_ALLOWANCE;
                }
                customers[current_id].playing_since = current_time;

        	}
            else if (!queue.empty()) // is anyone waiting?
            {
                current_id = queue.front();
                queue.pop_front();
                if (TIME_ALLOWANCE > customers[current_id].slots_remaining)
                {
                    time_out = current_time + customers[current_id].slots_remaining;
                }
                else
                {
                    time_out = current_time + TIME_ALLOWANCE;
                }
                customers[current_id].playing_since = current_time;
            }
        }
        print_state(out_file, current_time, current_id, arrival_events, queue);

        // exit loop when there are no new arrivals, no waiting and no playing customers
        all_done = (arrival_events.empty() && queue.empty() && (current_id == -1));
    }

    return 0;
}
