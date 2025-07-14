#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <optional>

namespace threading::utility
{
	template<typename T>
	class ConcurrentQueue
	{
	private:
		std::queue<T> data;
		std::atomic_size_t dataSize;
		mutable std::recursive_mutex dataMutex;

	public:
		ConcurrentQueue() = default;

		ConcurrentQueue(const ConcurrentQueue&) = delete;

		ConcurrentQueue(ConcurrentQueue&& other) noexcept;

		ConcurrentQueue& operator =(const ConcurrentQueue&) = delete;

		ConcurrentQueue& operator = (ConcurrentQueue&& other) noexcept;

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
		*/
		std::optional<T> pop();

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

		/**
		 * @brief Clear queue
		*/
		void clear();

		~ConcurrentQueue() = default;
	};

	template<typename T>
	ConcurrentQueue<T>::ConcurrentQueue(ConcurrentQueue&& other) noexcept
	{
		(*this) = std::move(other);
	}

	template<typename T>
	ConcurrentQueue<T>& ConcurrentQueue<T>::operator = (ConcurrentQueue<T>&& other) noexcept
	{
		data = std::move(other.data);
		dataSize = other.dataSize.load();

		other.dataSize = 0;

		return *this;
	}

	template<typename T>
	void ConcurrentQueue<T>::push(const T& value)
	{
		std::unique_lock<std::recursive_mutex> lock(dataMutex);

		data.push(value);

		dataSize++;
	}

	template<typename T>
	void ConcurrentQueue<T>::push(T&& value)
	{
		std::unique_lock<std::recursive_mutex> lock(dataMutex);

		data.push(std::move(value));

		dataSize++;
	}

	template<typename T>
	std::optional<T> ConcurrentQueue<T>::pop()
	{
		std::optional<T> result;

		{
			std::unique_lock<std::recursive_mutex> lock(dataMutex);

			if (data.empty())
			{
				return result;
			}

			result = std::move(data.front());

			data.pop();

			dataSize--;
		}

		return result;
	}

	template<typename T>
	size_t ConcurrentQueue<T>::size() const
	{
		return dataSize;
	}

	template<typename T>
	bool ConcurrentQueue<T>::empty() const
	{
		return !this->size();
	}

	template<typename T>
	void ConcurrentQueue<T>::clear()
	{
		std::unique_lock<std::recursive_mutex> lock(dataMutex);

		while (dataSize)
		{
			data.pop();
			dataSize--;
		}
	}
}
