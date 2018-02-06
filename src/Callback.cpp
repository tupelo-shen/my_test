// Copyright (c) 2016 XXXXXXXX Electric Corporation. All rights reserved.
/**
* @file Callback.cpp
* 
* @author   
* @version
*/
#include "Callback.h"

/*
 * Constructor
 */
Callback::Callback(int type, void (*func)(int), int client_data) : 
m_func(func),
m_type(type),
m_client_data(client_data),
m_next(0)
{

}

/*
 * Destructor
 */
Callback::~Callback()
{
}

/****************************************************************************/
/**
 * @brief   
 *
 * @author  
 * @date   
 *
 * @param       なし
 *
 * @return      なし
 */
/****************************************************************************/
void Callback::DoCallback()
{
    if(m_func) 
    {
        m_func(m_client_data);
    }
}