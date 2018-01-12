/*! \file  Mutex.hpp
    \brief Mutex definition.
    
    All of OS-specific Mutex object is implemented along with this definition.
    This class is based on boost::mutex.
*/

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <stdint.h>

namespace ana_stl {

	class Mutex {
		private:
			void *mtx; /*!< OS-specific Mutex object pointer */

		public:
			/******************************************************************************/
			/*!
				\brief     Constructor.
				\details   Constructor.
				\return    None
			 *******************************************************************************/
		Mutex();

		/******************************************************************************/
		/*!
			\brief     Destructor.
			\details   If member variable has any object, You have to delete.
			\return    None
		 *******************************************************************************/
		~Mutex();

		/******************************************************************************/
		/*!
			\brief      Set OS-specific Mutex object pointer to first argument.
			\details    Argument is defined as void pointer. You need to cast to specific type/class.
			\param[out] Mutex object pointer.
			\return     None
		 *******************************************************************************/
		void set(void **_mtx);

		/******************************************************************************/
		/*!
			\brief      Get OS-specific Mutex id pointer.
			\details    Return value is defined as uint32_t pointer. You need to cast to specific type/class.
			\return     Mutex id pointer
		 *******************************************************************************/
		uint32_t *getId();
	};
};

#endif
