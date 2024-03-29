// Vagabond : bond-based macromolecular model refinement
// Copyright (C) 2017-2018 Helen Ginn
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

#include "RefinementGridSearch.h"
#include "RefinementStepSearch.h"
#include "RefinementNelderMead.h"
#include "RefinementStrategy.h"
#include "FileReader.h"
#include <iostream>
#include <iomanip>

RefinementStrategy::RefinementStrategy()
{
	_enough = false;
	evaluationFunction = NULL;
	_partial = NULL;
	maxCycles = 30;
	cycleNum = 0;
	startingScore = 0;
	_verbose = false;
	_silent = false;
	_changed = -1;
	finishFunction = NULL;
	_mock = false;
	_improvement = 0;
	_toDegrees = false;
	_stream = &std::cout;
}

void RefinementStrategy::addParameter(void *object, Getter getter, Setter setter, double stepSize, double otherValue, std::string tag, Getter gradient)
{
	if (object == NULL)
	{
		return;
	}

	Parameter param;
	param.object = object;
	param.getter = getter;
	param.gradient = gradient;
	param.setter = setter;
	param.step_size = stepSize;
	param.other_value = otherValue;

	if (!tag.length())
	{
		tag = "object" + i_to_str((int)parameterCount());
	}

	param.tag = tag;
	param.coupled = 1;

	_params.push_back(param);
}

void RefinementStrategy::addCoupledParameter(void *object, Getter getter, Setter setter, double stepSize, double stepConvergence, std::string tag)
{
	int last = parameterCount() - 1;
	_params[last].coupled++;
	addParameter(object, getter, setter, stepSize, stepConvergence, tag);
	_params[last + 1].coupled++;
}

double RefinementStrategy::estimateGradientForParam(int i)
{
	double curr = getValueForParam(i);
	double step = _params[i].other_value;
	double right = curr + step / 2;
	setValueForParam(i, right);
	double right_val;
	
	if (_partial == NULL)
	{
		right_val = (*evaluationFunction)(evaluateObject);
	}
	else
	{
		right_val = (*_partial)(evaluateObject, _params[i].object);
	}

	double left = curr - step / 2;
	setValueForParam(i, left);
	double left_val;
	if (_partial == NULL)
	{
		left_val = (*evaluationFunction)(evaluateObject);
	}
	else
	{
		left_val = (*_partial)(evaluateObject, _params[i].object);
	}
	
	double diff = right_val - left_val;
	diff /= step;
	setValueForParam(i, curr);
	
	return diff;
}

double RefinementStrategy::getGradientForParam(int i)
{		
	Getter gradient = _params[i].gradient;
	
	if (!gradient)
	{
		return estimateGradientForParam(i);
	}
	void *object = _params[i].object;
	double grad = (*gradient)(object);
	
	return grad;
}

double RefinementStrategy::getValueForParam(int i)
{
	Getter getter = _params[i].getter;
	void *object = _params[i].object;
	double objectValue = (*getter)(object);

	return objectValue;
}

void RefinementStrategy::setValueForParam(int i, double value)
{
	Setter setter = _params[i].setter;
	void *object = _params[i].object;
	(*setter)(object, value);
}

void RefinementStrategy::refine()
{
	if (!jobName.length())
	{
		jobName = "Refinement procedure for " 
		+ i_to_str((int)parameterCount()) + " objects";
	}

	if (parameterCount() == 0)
	{
		std::cout << " No parameters to refine! Exiting." << std::endl;
		return;
	}

	if (evaluationFunction == NULL || evaluateObject == NULL)
	{
		std::cout << "Please set evaluation function and object." << std::endl;
		return;
	}

	startingScore = (*evaluationFunction)(evaluateObject);
	_prevScore = startingScore;

	for (size_t i = 0; i < parameterCount(); i++)
	{
		double value = getValueForParam(i);
		_params[i].start_value = value;
	}

	reportProgress(startingScore);
}

void RefinementStrategy::reportProgress(double score)
{
	if (!_verbose)
	{
		return;
	}
	
	if (cycleNum > 0 && cycleNum % 30 == 0)
	{
		double reduction = (startingScore - score) / startingScore;
		*_stream << std::setprecision(4);
		*_stream << " (" << std::fixed << -reduction * 100 << "%)";

		*_stream << std::endl;
	}

	if (score < _prevScore)
	{
		*_stream << "+" << std::flush;
	}
	else
	{
		*_stream << "." << std::flush;
	}
	
	_prevScore = score;
	
	cycleNum++;
}

void RefinementStrategy::finish()
{
	double endScore = (*evaluationFunction)(evaluateObject);
	
	if (!parameterCount())
	{
		return;
	}
	
	*_stream << std::setprecision(4);

	if (endScore >= startingScore || endScore != endScore)
	{
		resetToInitialParameters();
		_changed = 0;

		if (!_silent)
		{
			double rad2degscale = (_toDegrees ? (180 / M_PI) : 1);
			*_stream << "No change for " << jobName << " ";

			for (size_t i = 0; i < parameterCount(); i++)
			{
				double value = getValueForParam(i);
				_params[i].changed = 0;
				*_stream << _params[i].tag << "=" << value * rad2degscale <<
				(_toDegrees ? "º" : "") << ", ";
			}

			*_stream << " (" << startingScore << ") ";
			_timer.quickReport();
			*_stream << std::endl;
		}
	}
	else
	{
		double reduction = (startingScore - endScore) / startingScore;
		_improvement = -reduction * 100;

		if (!_silent)
		{*_stream << "Reduction ";
			double rad2degscale = (_toDegrees ? (180 / M_PI) : 1);

			if (reduction == reduction)
			{
				*_stream << "by " << std::fixed << 
				-reduction * 100 << "% ";
			}

			*_stream << "for " << jobName << ": ";

			for (size_t i = 0; i < parameterCount(); i++)
			{
				double value = getValueForParam(i);
				double start = _params[i].start_value;
				
				_params[i].changed = (fabs(start - value) > 1e-4);
				
				*_stream << _params[i].tag << "=" << value * rad2degscale <<
				(_toDegrees ? "°" : "") << ", ";
			}

			*_stream << "(" << startingScore << " to " << 
			endScore << ") ";
			_timer.quickReport();
			*_stream << std::endl;
		}

		_changed = 1;
		
		findIfSignificant();
	}

	cycleNum = 0;

	if (finishFunction != NULL)
	{
		(*finishFunction)(evaluateObject);
	}
}

void RefinementStrategy::resetToInitialParameters()
{
	for (size_t i = 0; i < parameterCount(); i++)
	{
		double value = _params[i].start_value;
		setValueForParam(i, value);
	}
}

void RefinementStrategy::reportResult()
{
	double val = improvement();

	if (didChange())
	{
		_changed = true;
		*_stream << std::setw(3) << val << "% improved. ... done. ";
	}
	else
	{
		*_stream << " not improved.   ... done. ";
	}
}


void RefinementStrategy::findIfSignificant()
{
	_enough = false;

	for (size_t i = 0; i < parameterCount(); i++)
	{
		Parameter p = getParamObject(i);
		double now = (*p.getter)(p.object);
		double change = fabs(now - p.start_value);
		
		if (change > p.other_value * 2)
		{
			_enough = true;
		}
	}

}

void RefinementStrategy::outputStream()
{
	std::ostringstream *o = static_cast<std::ostringstream *>(_stream);
	std::cout << o->str();
	o->str("");
}
