#include <condition_variable>
#include <vector>
#include <thread>
#include <type_traits>
#include <queue>
#include <functional>
#include <mutex>
#include <algorithm>
#include <memory>
#include <future>

class ThreadPool
{

public:
	ThreadPool(size_t size);
	~ThreadPool();
	template<class F, typename ... Args>
	decltype(auto) Enqueue(F&&, Args&& ... args); // managing task queque
private:
	std::vector<std::thread> workers; // workers
	std::queue<std::function<void()>> tasks; //task queue
	std::condition_variable cond; 
	std::mutex mtx; 
	bool stop;  //flag to stop the thread pool
};

inline ThreadPool::ThreadPool(size_t size) :stop(false)
{
	for (int i = 0; i < size; i++)
	{
		workers.emplace_back([this]()
		{
			while (true) 
			{
				std::function<void()> task;
				{   
					//starting of the critical section, waiting for new tasks or the destruction of the thread pool
					std::unique_lock<std::mutex> lock(mtx);
					cond.wait(lock, [this] { return this->stop || !this->tasks.empty(); }); 

					if (this->stop && this->tasks.empty())
						return; //return when the tasks queue is empty and the thread pool is flagged to stop

					task = std::move(this->tasks.front()); // get a new task
					this->tasks.pop(); // pop a task

					// mtx is automatically unlock at the end of the critical section
				}
					
				task(); //compute the task
			}
		});
	}
}

inline ThreadPool::~ThreadPool()
{
	{
		//lock to set the flag to true.
		std::unique_lock<std::mutex> lock(mtx);
		stop = true;
		//automatically unlock mtx
	}
	cond.notify_all();// notify all thread waiting for this condition variable 
	for_each(workers.begin(), workers.end(), std::mem_fn(&std::thread::join)); // wait for threads to finish
}


template<class F, typename ...Args>
decltype(auto) ThreadPool::Enqueue(F&& f, Args && ...args)
{
	using return_type = typename std::result_of_t<F(Args...)>; // get the result type

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...) // binding functions and the relative arguments
		);

	std::future<return_type> res = task->get_future();
	
	{
		//lock the task queque
		std::unique_lock<std::mutex> lock(mtx);
		
		//if the thread pool has been destructed, it is not allowed to enqueque any more.
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	cond.notify_one(); //notify one thread to receive further tasks
	return res;
}


