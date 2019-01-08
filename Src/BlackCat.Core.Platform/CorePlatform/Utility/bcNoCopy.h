// [03/23/2016 MRB]

#pragma once

namespace black_cat
{
	namespace core_platform
	{
		class bc_no_copy
		{
		public:
			bc_no_copy(const bc_no_copy&) = delete;

			bc_no_copy(bc_no_copy&&) = default;

			bc_no_copy& operator=(const bc_no_copy&) = delete;

			bc_no_copy& operator=(bc_no_copy&&) = default;

		protected:
			bc_no_copy() = default;

			~bc_no_copy() = default;

		private:
		};

		class bc_no_copy_move : public bc_no_copy
		{
		public:
			bc_no_copy_move(bc_no_copy_move&&) = delete;

			bc_no_copy_move& operator=(bc_no_copy_move&&) = delete;

		protected:
			bc_no_copy_move() = default;

			~bc_no_copy_move() = default;

		private:
		};
	}
}