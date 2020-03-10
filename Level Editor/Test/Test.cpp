#include <TXLib.h>

//-----------------------------------------------------------------------------

struct Queue

{

    static const int QUEUE_LENGTH = 25;

    char symbols_ [QUEUE_LENGTH];

    int head_;

    int tail_;

    void pushBack (char symbol);

    char popFront ();

    void draw ();

    Queue ();

};

Queue::Queue () :

    symbols_ (""),
    head_    (0),
    tail_    (0)

{}

//-----------------------------------------------------------------------------

int main ()

{

    $C;

    Queue q = {};

    while (!GetAsyncKeyState (VK_ESCAPE))

    {

        txClearConsole ();

        q.draw ();

        if (kbhit ());

        {

            char key = getch();

            if (key == '\t')

            {

                printf ("\npopfront = %c", q.popFront ());

            }

            else q.pushBack (key);

        }

        txSleep (1);

    }

}

//-----------------------------------------------------------------------------

void Queue::pushBack (char symbol)

{

    symbols_ [head_] = symbol;

    head_ ++;

    if (head_ > QUEUE_LENGTH) head_ = 0;

}

//-----------------------------------------------------------------------------

char Queue::popFront ()

{

    char symbol = symbols_ [tail_];

    symbols_ [tail_] = '\0';

    tail_ ++;

    if (tail_ > QUEUE_LENGTH) tail_ = 0;

    return symbol;

}

//-----------------------------------------------------------------------------

void Queue::draw ()

{

    for (int i = 0; i <= QUEUE_LENGTH; i++)

    {

        assert (0 <= i && i <= QUEUE_LENGTH);

        printf ("%02d ", i);

    }

    printf ("\n");

    for (int i = 0; i <= QUEUE_LENGTH; i++)

    {

        assert (0 <= i && i <= QUEUE_LENGTH);

        printf ("%2c ", symbols_ [i]);

    }

    printf ("\n");

    for (int i = 0; i <= QUEUE_LENGTH; i++)

    {

        if (i == head_ && i != tail_)

        {

            printf (" h ");

        }

        else if (i == tail_ && i != head_)

        {

            printf (" t ");

        }

        else if (i == tail_ && i == head_)

        {

            printf ("ht ");

        }

        else printf ("   ");

    }

    printf ("\nhead = %d", head_);
    printf ("\ntail = %d", tail_);

}
