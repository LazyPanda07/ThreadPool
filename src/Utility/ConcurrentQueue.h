#pragma once

#include <queue>
#include <mutex>

namespace threading
{
	namespace utility
	{
		template<typename T>
		class ConcurrentQueue
		{
		private:
			std::queue<T> data;
			mutable std::mutex dataMutex;

		public:
			ConcurrentQueue() = default;

			/**
			 * @brief Add element to queue
			 * @param value New element
			*/
			void push(const T& value);

			/**
			 * @brief Add element to queue
			 * @param value New element
			*/
			void push(T&& value);

			/**
			 * @brief Give out first element from queue
			 * @return First element in queue
			 * @exception std::runtime_error
			*/
			T pop();

			/**
			 * @brief Current size of queue
			 * @return Queue size
			*/
			size_t size() const;

			/**
			 * @brief Checks whether the queue is empty
			 * @return 
			*/
			bool empty() const;

			~ConcurrentQueue() = default;
		};

		template<typename T>
		void ConcurrentQueue<T>::push(const T& value)
		{
			std::unique_lock<std::mutex> lock(dataMutex);

			data.push(value);
		}

		template<typename T>
		void ConcurrentQueue<T>::push(T&& value)
		{
			std::unique_lock<std::mutex> lock(dataMutex);

			data.push(std::move(value));
		}

		template<typename T>
		T ConcurrentQueue<T>::pop()
		{
			std::unique_lock<std::mutex> lock(dataMutex);

			if (data.empty())
			{
				throw std::runtime_error("No front element");
			}

			T value = std::move(data.front());

			data.pop();

			return value;
		}

		template<typename T>
		size_t ConcurrentQueue<T>::size() const
		{
			std::unique_lock<std::mutex> lock(dataMutex);

			return data.size();
		}

		template<typename T>
		bool ConcurrentQueue<T>::empty() const
		{
			return !this->size();
		}
	}
}
