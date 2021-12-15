#include <Gridworld/GridData.h>
#include <exception>

namespace gridworld_cybsys
{

	GridData::GridData()
		: Columns(0), Rows(0)
	{
	}

	GridData::GridData(unsigned columns, unsigned rows)
		: Columns(columns), Rows(rows), Data(rows*columns, ' ')
	{
	}


	void GridData::SetDimension(unsigned columns, unsigned rows)
	{
		Columns = columns;
		Rows = rows;
		Data.resize(Rows*Columns, ' ');
	}

	void GridData::Clear()
	{
		Data.resize(Rows*Columns, ' ');
	}


	bool GridData::CheckPosition(unsigned column, unsigned row) const
	{
		if (row >= Rows || column >= Columns)
		{
			return false;
		}
		return true;
	}


	std::string GridData::GetRow(unsigned row) const
	{
		if (row >= Rows)
		{
			throw std::out_of_range("GridData: row out of range.");
		}
		return Data.substr(row*Columns, Columns);
	}


	bool GridData::SetRow(unsigned row, std::string data)
	{
		if (!CheckPosition((unsigned)data.size() - 1U, row))
		{
			return false;
		}
		Data.replace(Columns*row, (unsigned)data.size(), data);

		return true;
	}


	bool GridData::SetRows(const std::vector<std::string>& data)
	{
		if (!CheckPosition((unsigned)data.size() - 1U,0))
		{
			return false;
		}
		for (unsigned row = 0; row < data.size(); row++)
		{
			const std::string& rowString = data.at(row);
			if (!CheckPosition(row, (unsigned)rowString.size() - 1U))
			{
				return false;
			}
		}
		for (unsigned row = 0; row < data.size(); row++)
		{
			const std::string& rowString = data.at(row);
			Data.replace(Columns*row, rowString.size(), rowString);
		}
		return true;
	}


	bool GridData::SetData(const std::string& data)
	{
		if (Data.size() != data.size())
		{
			return false;
		}
		Data = data;
		return true;
	}


	const char GridData::GetCell(const GridPosition& pos) const
	{
		return GetCell(pos.Column,pos.Row);
	}


	const char GridData::GetCell(unsigned column, unsigned row) const
	{
		if (row >= Rows)
		{
			throw std::out_of_range("GridData: row out of range.");
		}
		if (column >= Columns)
		{
			throw std::out_of_range("GridData: column out of range.");
		}
		return Data[row*Columns + column];
	}

}
