#include <kmain.h>
#include <schedule.h>
#include <sem.h>

int STOP = 10000;

TCB_TypeDef task[22],
    _sleep, _sleepDead, task_FCFS[22], task_RR[22];
int count = 0;
int x = 10;

end_task[task_count] = {0};
void task_sleep(void);
void task_sleep_dead(void);
void Timetable(int n);
void Task(void);
int semaphore = 0;
void kmain(void)
{
    __sys_init();

    //__SysTick_init(10000);

    // /__SysTick_init(1000000);	//enable systick for 100ms

    while (1)
    {
        int a = 4;

        kprintf(".........................Lab 4..................\n");
        kprintf("Press 4 for Lab 4\n");
        // kscanf("%d", &a);

        if (a == 4)
        {
            Lab4();
        }
    }
}

void Semaphore();
void Lab4(void)
{

    kprintf(".........................Lab 4..................\n");
    __NVIC_SetPriority(SVCall_IRQn, 1);
    __NVIC_SetPriority(SysTick_IRQn, 2);
    __NVIC_SetPriority(PendSV_IRQn, -12);

    int x;
    kprintf("Press 1 for Syncronization\n");
    uscanf("%d", &x);
    if (x == 1)
        Semaphore();

    while (1)
        ;
}

void Semaphore()
{
    for (int i = 0; i < task_count; i++)
    {
        task_create(task + i, Task, (uint32_t *)(TASK_STACK_START - (i * TASK_STACK_SIZE)));
    }

    task_create(&_sleep, task_sleep, (uint32_t *)(TASK_STACK_START - (task_count * TASK_STACK_SIZE)));

    initialize_queue();

    for (int i = 0; i < task_count; i++)
        add_to_ready_queue(task + i);
    set_sleeping_task(&_sleep);
    set_task_pending(1);
    task_start();
    return;
};

void PrintAll()
{

    uint32_t av_response = 0, avg_waiting = 0, avg_turnaround = 0, avg_execution = 0;
    for (int i = 0; i < task_count; i++)
    {
        av_response += task[i].reponse_time_t;
        avg_waiting += task[i].waiting_time_t;
        avg_turnaround += task[i].turn_time;
        avg_execution += task[i].execution_time_t;

        printf("\n\nID: %d\nArrival Time: %dms\nResponse Time: %dms\nWaiting Time: %dms\nExecution Time: %dms\nTurn Around %dms\n", task[i].task_id - 1000, task[i].start_time_t, task[i].reponse_time_t, task[i].waiting_time_t, task[i].execution_time_t, task[i].turn_time);
    }
}
void task_sleep(void)
{
    int reboot_ = 0;
    PrintAll();
    printf("\n.......................\nCompleted....................\nPress 1 to reboot.\n\r");

    uscanf("%d", &reboot_);
    if (reboot_ == 1)

        reboot();

    if (reboot_ == 1)
        ;
}

void Task(void)
{
    TCB_TypeDef taski = getpid(); /* It is an SVC call*/

    task[taski.task_id - 1000].reponse_time_t = __getTime() - task[taski.task_id - 1000].start_time_t;
    uint32_t value;
    uint32_t inc_count = 0;

    wait(&semaphore);
    while (1)
    {
        // wait(&semaphore);

        value = count;
        value++;

        if (value != count + 1)
        {
            printf("Task %d ", taski.task_id);
            printf("Error %d != %d\n\r", value, count + 1); /* It is an SVC call*/
        }
        else
        {
            count = value;
            inc_count++;
        }

        if (count >= STOP)
        {
            printf("Total increment done by task %d is: %d\n\r", taski.task_id, inc_count);
            printf("Total increment done by task is: %d\n\r", inc_count);

            break;
        }
    }
    signal(&semaphore);

    int index = taski.task_id - 1000;
    end_task[index] = 1;
    task[taski.task_id - 1000].turn_time = __getTime() - task[taski.task_id - 1000].start_time_t;

    exit();
}

void unprivileged_mode(void)
{
    __asm volatile("MRS R0, CONTROL");
    __asm volatile("ORRS R0, R0, #1");
    __asm volatile("MSR CONTROL, R0");
}
