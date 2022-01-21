// Vagabond
// Copyright (C) 2019 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

/* holds a pointer and will provide getters and setters for any
 * double of your choice */

typedef void (*Refresh)(void *);

class Any
{
public:
	Any(double *ptr, double scale = 1)
	{
		_ptr = ptr;
		_scale = scale;
		_initial = *ptr;
		_value = 0;
		_g = NULL;
		_gobj = NULL;
	}
	
	static double get(void *object)
	{
		return (static_cast<Any *>(object)->_value);
		return *(static_cast<Any *>(object)->_ptr);
	}
	
	void pset(double val)
	{
		_value = val;
		double aim = _initial + val * _scale;
		*_ptr = aim;

		if (_g != NULL)
		{
			(*_g)(_gobj);
		}
	}

	static void set(void *object, double val)
	{
		Any *any = static_cast<Any *>(object);
		any->pset(val);
	}
	
	void setRefresh(Refresh g, void *object)
	{
		_g = g;
		_gobj = object;
	}

private:
	double *_ptr;
	double _initial;
	double _value;
	double _scale;
	Refresh _g;
	void *_gobj;

};

