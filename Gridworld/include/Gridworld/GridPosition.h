#pragma once

namespace gridworld_cybsys
{
	struct GridPosition
	{
		unsigned Column = 0U;
		unsigned Row = 0U;

		GridPosition()
		{
		}

		GridPosition(unsigned column, unsigned row)
			: Column(column), Row(row)
		{
		}

		bool operator ==(const GridPosition& gridPosition) const
		{
			return Column == gridPosition.Column
				&& Row == gridPosition.Row;
		}
	};

}
