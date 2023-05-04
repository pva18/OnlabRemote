template <typename T, int BUFFER_SIZE>
class CircularBuffer
{
    T buffer[BUFFER_SIZE];
    int head = 0;
    int tail = 0;

public:
    bool enqueue(T value)
    {
        int next = (head + 1) % BUFFER_SIZE;
        if (next != tail)
        {
            buffer[head] = value;
            head = next;
            return true;
        }
        return false;
    }

    bool dequeue(T *value)
    {
        if (tail == head)
        {
            return false;
        }
        *value = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        return true;
    }

    const T *peek()
    {
        return buffer[tail];
    }

    int size()
    {
        return (head + BUFFER_SIZE - tail) % BUFFER_SIZE;
    }
};
