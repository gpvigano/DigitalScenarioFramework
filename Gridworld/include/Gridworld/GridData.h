#pragma once

#include <vector>
#include <memory>
#include "GridPosition.h"

namespace gridworld_cybsys
{
	class GridData
	{
	public:

		GridData();

		GridData(unsigned columns, unsigned rows);

		void SetDimension(unsigned columns, unsigned rows);
		unsigned GetNumRows() const { return Rows; }
		unsigned GetNumColumns() const { return Columns; }
		bool IsValid() const { return Rows>0 && Columns>0; }
		void Clear();

		std::string GetRow(unsigned row) const;
		bool SetRow(unsigned row, std::string data);
		bool SetRows(const std::vector<std::string>& data);
		bool SetData(const std::string& data);

		const char GetCell(const GridPosition& pos) const;
		const char GetCell(unsigned column, unsigned row) const;

	protected:
		unsigned Columns = 0U;
		unsigned Rows = 0U;
		std::string Data;

		bool CheckPosition(unsigned column, unsigned row) const;
	};
}
