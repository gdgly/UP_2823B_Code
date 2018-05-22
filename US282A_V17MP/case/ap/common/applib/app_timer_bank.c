/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * ����������ʱ������ģ�飬������ʼ�����������޸ģ�ֹͣ��������ɾ������ǰ̨Ӧ��ʹ�á�
 * ���ߣ�cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

static inline int8 __set_timer(timer_attrb_e attrb, timer_type_e type, uint8 tag, uint16 timeout, timer_proc func_proc);
static inline bool __set_timer_twinkle(int8 timer_id, twinkle_app_timer_param_t *twinkle_param);

/******************************************************************************/
/*!
 * \par  Description:
 *    ������ͨӦ�ü���ʱ����
 * \param[in]    attrb ��ʱ�����ԣ�������һ�ַ��࣬�� control �� ui ��
 * \param[in]    timeout ��ʱ�����ڣ��� 1ms Ϊ��λ������Ϊ 0
 * \param[in]    func_proc ��ʱ���������̣�����Ϊ NULL
 * \param[out]   none
 * \return       int8
 * \retval           id ����ɹ������ض�ʱ��ID����ΧΪ 0 ~ g_app_timer_count - 1��
 * \retval           -1 ����ʧ�ܣ���ʱ����Դ���꣬�� timeout Ϊ0���� proc ΪNULL��
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1������ 80ms �ַ���������ʱ��
 //������ʱ�� ISR
 static void __scroll_string_handle(void)
 {
 ui_show_listbox(NULL, NULL, LIST_DRAW_ACTIVE_SCROLL);
 }

 int8 scroll_timer = -1;
 //���� UI�� Ӧ�ü���ʱ����������ʱ������
 scroll_timer = set_app_timer(APP_TIMER_ATTRB_UI, 80, __scroll_string_handle);
 if(scroll_timer == -1)
 {
 ����ʧ�ܣ�ԭ��Ϊ��ʱ���ѱ�������
 }

 //ɾ����ЧӦ�ü���ʱ��
 if(scroll_timer != -1)
 {
 kill_app_timer(scroll_timer);
 scroll_timer = -1;
 }
 * \endcode
 * \note
 * \li  ��ͨ���͵�����ʱ����ÿ�ζ�ʱ�������ú��´ζ�ʱʱ�䣬Ȼ��ִ�� ISR���Զ�����ִ�У�ֱ���û�����
 *      stop_app_timer ���� kill_app_timer��
 * \li  ���� set_app_timer ������ʱ���󣬱����ڲ���ʹ��ʱ���� kill_app_timer ɾ��֮����������ڴ�й©��
 * \li  timeout����Ϊ0��func_proc����Ϊ�գ����򷵻�ʧ�ܡ�
 *******************************************************************************/
int8 set_app_timer(uint32 attrb_tagh, uint16 timeout, timer_proc func_proc)
{
    return __set_timer(attrb_tagh & 0xff, TIMER_TYPE_NORMAL, (uint8) (attrb_tagh >> 8), timeout, func_proc);
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ��������Ӧ�ü���ʱ����
 * \param[in]    attrb ��ʱ�����ԣ�������һ�ַ��࣬�� control �� ui ��
 * \param[in]    timeout ��ʱ�����ڣ��� 1ms Ϊ��λ
 * \param[in]    func_proc ��ʱ����������
 * \param[out]   none
 * \return       int8
 * \retval           id ����ɹ������ض�ʱ��ID����ΧΪ 0 ~ g_app_timer_count - 1��
 * \retval           -1 ����ʧ�ܣ���ʱ����Դ���꣬�� timeout Ϊ0���� proc ΪNULL��
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1������ 5s ��ʱ��ʱ������������ĳ���¼�
 //5s ��ʱ��ʱ�� ISR���ñ�־ g_timeout_flagd
 static void __timeout_handle(void)
 {
 g_timeout_flagd = TRUE;
 }

 bool g_timeout_flagd = FALSE;//ȫ�ֱ���
 int8 timeout_timer = -1;
 //���� control�� Ӧ�ü���ʱ��
 timeout_timer = set_single_shot_app_timer(APP_TIMER_ATTRB_UI, 5000, __timeout_handle);
 if(timeout_timer == -1)
 {
 ����ʧ�ܣ�ԭ��Ϊ��ʱ���ѱ�������
 }

 //ɾ����ЧӦ�ü���ʱ��������ܹ�ȷ��������ʱ����ִ�е�����ô�Ͳ���Ҫִ��ɾ��������
 if(timeout_timer != -1)
 {
 kill_app_timer(timeout_timer);//����ڴ�֮ǰ������ʱ��������ɾ�����᷵�ش��󣬵��ǲ������
 timeout_timer = -1;
 }
 * \endcode
 * \note
 * \li  ����Ӧ�ü���ʱ����Ψһһ�� ISR ִ��ʱ�ͱ��Զ����٣��û����Բ����Լ����١�
 * \li  �������ֶ�ʱ��Ҳ�����⣬�����û�������ȫ���ն�ʱ�����������ڣ�����ȷ����ʱ��
 *      �Ƿ������ˣ��������п����ظ�������ͬ�Ķ�ʱ����
 * \li  timeout����Ϊ0��func_proc����Ϊ�գ����򷵻�ʧ�ܡ�
 *******************************************************************************/
int8 set_single_shot_app_timer(uint32 attrb_tagh, uint16 timeout, timer_proc func_proc)
{
    return __set_timer(attrb_tagh & 0xff, TIMER_TYPE_SINGLE_SHOT, (uint8) (attrb_tagh >> 8), timeout, func_proc);
}

//����twinkle�ඨʱ��
int8 set_twinkle_app_timer(uint32 attrb_tagh, twinkle_app_timer_param_t *twinkle_param)
{
    int8 timer_id;

    //�ȴ���1��ȷ��__set_timer��ȷ����
    timer_id = __set_timer(attrb_tagh & 0xff, TIMER_TYPE_NORMAL, (uint8) (attrb_tagh >> 8), 1, (timer_proc) 1);
    if (timer_id == -1)
    {
        return timer_id;
    }

    __set_timer_twinkle(timer_id, twinkle_param);

    return timer_id;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    �޸�Ӧ�ü���ʱ���Ķ�ʱ���ڡ�
 * \param[in]    timer_id ��ʱ��ID
 * \param[in]    timeout ���µĶ�ʱ�����ڣ��� 1ms Ϊ��λ
 * \param[out]   none
 * \return       bool
 * \retval           TRUE �޸Ķ�ʱ���ڳɹ���
 * \retval           FALSE �޸Ķ�ʱ����ʧ�ܣ����Ҳ���ָ��ID�Ķ�ʱ������ timeout �Ƿ���
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1���޸Ķ�ʱ����ʱʱ��Ϊ 5s��֮�����¼�ʱ
 if(modify_app_timer(timer_id, 5000) == FALSE)
 {
 �޸�ʧ��
 }
 * \endcode
 * \note
 * \li  �޸�����ʱ���Ķ�ʱ���ں󣬻����¿�ʼ��ʱ��
 * \li  timerout����Ϊ0�����򷵻�ʧ�ܡ�
 * \li  timer_id ����0 ~ g_app_timer_count - 1�����з���FALSE��
 *******************************************************************************/
bool modify_app_timer(int8 timer_id, uint16 timeout)
{
    app_timer_t *tm;
    app_timer_t *timers;
    uint8 count;

    if ((timer_id & 0xf0) == 0x40)
    {
        timer_id &= 0x0f;
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }

    if ((timer_id == -1) || ((uint8) timer_id >= count)) //����Խ��
    {
        return FALSE;
    }

    tm = timers + timer_id;

    if (0 == timeout)
    {
        return FALSE;
    }

    tm->timeout = timeout;
    tm->timeout_expires = sys_get_ab_timer() + timeout;
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ֹͣӦ�ü���ʱ����ʱ������ʱ��״̬��Ϊ TIMER_STATE_STOPED��
 * \param[in]    timer_id ��ʱ��ID
 * \param[out]   none
 * \return       bool
 * \retval           TRUE ֹͣ��ʱ�ɹ���
 * \retval           FALSE ֹͣ��ʱʧ�ܣ�ԭ��Ϊ�Ҳ���ָ��ID�Ķ�ʱ����
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1��ֹͣ��ʱ����ʱ��֮���ͨ�� restart_app_timer �ָ�
 if(stop_app_timer(timer_id) == FALSE)
 {
 ֹͣʧ��
 }
 * \endcode
 * \note
 * \li  ֹͣ��ʱ�����ٻָ���ֹͣʱ��������ʱ��ֻ�ܵ��� restart_app_timer ���¼�ʱ��
 * \li  timer_id ����0 ~ g_app_timer_count - 1�����з���FALSE��
 *******************************************************************************/
bool stop_app_timer(int8 timer_id)
{
    app_timer_t *tm;
    app_timer_t *timers;
    uint8 count;

    if ((timer_id & 0xf0) == 0x40)
    {
        timer_id &= 0x0f;
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }

    if ((timer_id == -1) || ((uint8) timer_id >= count)) //����Խ��
    {
        return FALSE;
    }

    tm = timers + timer_id;

    tm->state = TIMER_STATE_STOPED;
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ����Ӧ�ü���ʱ����ʱ������״̬��Ϊ TIMER_STATE_RUNNING��
 * \param[in]    timer_id ��ʱ��ID
 * \param[out]   none
 * \return       bool
 * \retval           TRUE ������ʱ�ɹ���
 * \retval           FALSE ������ʱʧ�ܣ�ԭ��Ϊ�Ҳ���ָ��ID�Ķ�ʱ����
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1���ָ���ʱ����ʱ����ʱ������ͷ��ʼ��ʱ
 if(restart_app_timer(timer_id) == FALSE)
 {
 �ָ���ʱʧ��
 }
 * \endcode
 * \note
 * \li  ������ʱ�������¿�ʼһ����ʱ���ڡ�
 * \li  timer_id ����0 ~ g_app_timer_count - 1�����з���FALSE��
 *******************************************************************************/
bool restart_app_timer(int8 timer_id)
{
    app_timer_t *tm;
    app_timer_t *timers;
    uint8 count;

    if ((timer_id & 0xf0) == 0x40)
    {
        timer_id &= 0x0f;
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }

    if ((timer_id == -1) || ((uint8) timer_id >= count)) //����Խ��
    {
        return FALSE;
    }

    tm = timers + timer_id;

    tm->state = TIMER_STATE_RUNNING;

    if ((tm->twinkle == TRUE) && (tm->on_off == TRUE))
    {
        tm->timeout_expires = sys_get_ab_timer() + tm->timeout_off;
    }
    else
    {
        tm->timeout_expires = sys_get_ab_timer() + tm->timeout;
    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    ɾ��Ӧ�ü���ʱ��������ʱ��״̬��Ϊ TIMER_STATE_NOUSED��
 * \param[in]    timer_id ��ʱ��ID
 * \param[out]   none
 * \return       bool
 * \retval           TRUE ɾ����ʱ���ɹ���
 * \retval           FALSE ɾ����ʱ��ʧ�ܣ�ԭ��Ϊ�Ҳ���ָ��ID�Ķ�ʱ����
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1��ɾ����ʱ��
 if(kill_app_timer(timer_id) == FALSE)
 {
 ɾ����ʱ��ʧ��
 }
 * \endcode
 * \note
 * \li  timer_id ����0 ~ g_app_timer_count - 1�����з���FALSE��
 *******************************************************************************/
bool kill_app_timer(int8 timer_id)
{
    app_timer_t *tm;
    app_timer_t *timers;
    uint8 count;

    if ((timer_id & 0xf0) == 0x40)
    {
        timer_id &= 0x0f;
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }

    if ((timer_id == -1) || ((uint8) timer_id >= count)) //����Խ��
    {
        return FALSE;
    }

    tm = timers + timer_id;

    tm->state = TIMER_STATE_NOUSED;
    return TRUE;
}


bool kill_app_timer_safe(int8 timer_id, timer_proc time_handle)
{
    app_timer_t *tm;
    app_timer_t *timers;
    uint8 count;

    if ((timer_id & 0xf0) == 0x40)
    {
        timer_id &= 0x0f;
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }
    if ((timer_id == -1) || ((uint8) timer_id >= count)) //����Խ��
    {
        return FALSE;
    }
    tm = timers + timer_id;
    if(tm->func_proc == time_handle)
    {
        tm->state = TIMER_STATE_NOUSED;
    }
    
    return TRUE;
}




/******************************************************************************/
/*!
 * \par  Description:
 *    ��ʼ��Ӧ�ü���ʱ������ģ�飬�����ж�ʱ��״̬��Ϊ TIMER_STATE_NOUSED��
 * \param[in]    timers Ӧ�ò㶨�������ʱ������
 * \param[in]    count Ӧ�ò㶨�������ʱ�����������ʱ����Ŀ
 * \param[out]   none
 * \return       bool
 * \retval           TRUE ��ʼ���ɹ���
 * \retval           FALSE ��ʼ��ʧ�ܣ�ԭ��Ϊ�����Ƿ���
 * \ingroup      app_timer
 * \par          exmaple code
 * \code
 ����1����ʼ����ʱ������ģ��
 #define APP_TIMER_COUNT     2
 app_timer_t app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

 if(init_app_timers(app_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT) == FALSE)
 {
 ��ʼ����ʱ������ģ��ʧ�ܣ���������
 }
 * \endcode
 * \note
 * \li  ��Ӧ�� main ������ڴ����ã��ڴ�֮ǰ�������������� app_timer �ӿڡ�
 * \li  �ýӿ��ڽ���AP�󣬵����� applib_init �ӿں���á�
 *******************************************************************************/
bool init_app_timers(app_timer_t *timers, uint8 count)
{
    app_timer_t *tm;
    uint8 i;

    if (count == 0) //��ʱ����Ŀ����Ϊ0
    {
        return FALSE;
    }

    g_app_timer_vector = timers;
    g_app_timer_count = count;

    for (i = 0; i < count; i++)
    {
        tm = timers + i;
        tm->state = TIMER_STATE_NOUSED;
    }

    //��ʼ��Ϊ AP ����
    g_this_app_timer_tag = APP_TIMER_TAG_MAIN;

    return TRUE;
}

//����ʱ��STANDBY�˳�������ʱ
void standby_restart_all_app_timer(app_timer_t *timers, uint8 count)
{
    app_timer_t *tm;
    uint8 i;

    for (i = 0; i < count; i++)
    {
        tm = timers + i;

        if (tm->state == TIMER_STATE_RUNNING)
        {
            if ((tm->twinkle == TRUE) && (tm->on_off == TRUE))
            {
                tm->timeout_expires = sys_get_ab_timer() + tm->timeout_off;
            }
            else
            {
                tm->timeout_expires = sys_get_ab_timer() + tm->timeout;
            }
        }
    }
}

/*! \endcond */

/*! \cond */

//����Ӧ�ü���ʱ��
static inline int8 __set_timer(timer_attrb_e attrb, timer_type_e type, uint8 tag, uint16 timeout, timer_proc func_proc)
{
    app_timer_t *tm;
    int8 timer_id = -1;
    int8 i;
    bool com_app_timer_flag = FALSE;
    app_timer_t *timers;
    uint8 count;

    //�������̲���ΪNULL
    if (!func_proc)
    {
        return -1;
    }

    //��ʱ���ڲ���Ϊ0
    if (0 == timeout)
    {
        return -1;
    }

    if (attrb == APP_TIMER_ATTRB_COM_UI)
    {
        com_app_timer_flag = TRUE;
        attrb = APP_TIMER_ATTRB_UI;
    }
    if (attrb == APP_TIMER_ATTRB_COM_CONTROL)
    {
        com_app_timer_flag = TRUE;
        attrb = APP_TIMER_ATTRB_CONTROL;
    }

    if (com_app_timer_flag == TRUE)
    {
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }

    //�������������Ŀ�������ʱ��
    for (i = 0; i < count; i++)
    {
        if (timers[i].state == TIMER_STATE_NOUSED)
        {
            timer_id = i;
            tm = timers + timer_id;

            //��ʼ������ʱ��
            tm->timeout = timeout;
            tm->timeout_off = 0;
            tm->timeout_expires = sys_get_ab_timer() + timeout;
            tm->func_proc = func_proc;
            tm->state = TIMER_STATE_RUNNING;
            tm->attrb = attrb;
            tm->type = type;
            tm->twinkle = FALSE;
            tm->on_off = FALSE;
            tm->tag = tag;
            tm->count = 0;
            break;
        }
    }

    if ((com_app_timer_flag == TRUE) && (timer_id != -1))
    { //��4bit��Ϊ0100��ʾCOM����ʱ��
        timer_id |= 0x40;
    }

    if (timer_id == -1)
    {
        PRINT_ERR("set app timer fail#1");
        while (1)
        {
            ; //nothing
        }
    }

    return timer_id;
}

static inline bool __set_timer_twinkle(int8 timer_id, twinkle_app_timer_param_t *twinkle_param)
{
    app_timer_t *tm;
    app_timer_t *timers;
    uint8 count;

    if ((timer_id & 0xf0) == 0x40)
    {
        timer_id &= 0x0f;
        timers = g_com_app_timer_vct;
        count = COM_APP_TIMER_MAX;
    }
    else
    {
        timers = g_app_timer_vector;
        count = g_app_timer_count;
    }

    if ((timer_id == -1) || ((uint8) timer_id >= count)) //����Խ��
    {
        return FALSE;
    }

    tm = timers + timer_id;

    tm->twinkle = TRUE;
    tm->on_off = twinkle_param->on_off;
    tm->count = twinkle_param->count;
    tm->timeout = twinkle_param->timeout_on;
    tm->timeout_off = twinkle_param->timeout_off;
    tm->func_proc = twinkle_param->func_proc;

    return TRUE;
}

/*! \endcond */