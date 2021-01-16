// blasty
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

#ifndef __abmap__blast__
#define __abmap__blast__

#include <cstring>
#include <cmath>
#include <iostream>

#define MATCHED -1
#define UNTOUCHED 9

typedef struct
{
	std::string seq;
	char *mask;
	size_t *map;

} Alignment;

size_t mask_length(Alignment al, size_t threshold)
{
	size_t total = 0;

	for (size_t i = 0; i < al.seq.length(); i++)
	{
		if (al.mask[i] == UNTOUCHED || 
		    al.mask[i] > (char)threshold)
		{
			total++;
		}
	}
	
	return total;
}

int choose_seed(Alignment al, size_t threshold)
{
	size_t length = mask_length(al, threshold);
	
	if (length == 0)
	{
		return -1;
	}

	size_t seed = rand() % length;
	
	size_t count = 0;
	for (size_t i = 0; i < al.seq.length(); i++)
	{
		bool acceptable = (al.mask[i] == UNTOUCHED || 
		                   al.mask[i] > (char)threshold);

		if (!acceptable)
		{
			seed++;
		}
		
		if (i == seed)
		{
			count = i;
			break;
		}
	}
	
	return count;
}

/* gets alignment limits from other side */
void find_limits(Alignment ala, Alignment alb, size_t pos,
                 size_t *low, size_t *high, double *pct)
{
	*low = 0;
	*high = alb.seq.size();
	int low_idx = 0;
	int high_idx = ala.seq.size();

	for (size_t i = pos; i < ala.seq.size(); i++)
	{
		if (ala.map[i] != std::string::npos)
		{
			*high = ala.map[i];
			high_idx = i;
			break;
		}
	}

	for (int i = pos; i >= 0; i--)
	{
		if (ala.map[i] != std::string::npos)
		{
			*low = ala.map[i];
			low_idx = i + 1;
			break;
		}
	}
	
	*pct = (pos - low_idx) / (high_idx - low_idx);
}

size_t best_match(Alignment ala, Alignment alb, size_t pos, int threshold)
{
	int best_score = -2;
	size_t bstart = 0;
	size_t length = 0;
	size_t astart = 0;
	double best_pct_diff = 1;
	
	size_t low = 0;
	size_t high = alb.seq.length();
	double pct = 0;
	find_limits(ala, alb, pos, &low, &high, &pct);
	
	for (size_t i = low; i < high; i++)
	{
		if (alb.seq[i] != ala.seq[pos])
		{
			continue;
		}
		
		int count = -2; // two auto-hits will happen

		int as = pos;
		int bs = i;
		for (int j = (int)i; j >= 0; j--)
		{
			if (alb.seq[j] != ala.seq[as] 
			    || ala.mask[as] == MATCHED
			    || alb.mask[j] == MATCHED)
			{
				bs++; as++;
				break;
			}
			
			count++;
			bs--; as--;
			
			if (as < 0 || bs < 0)
			{
				as++;
				bs++;
				break;
			}
		}
		
		int ae = pos;
		int be = i;
		for (size_t j = i; j < alb.seq.size(); j++)
		{
			if (alb.seq[j] != ala.seq[ae] || 
			    ala.mask[ae] == MATCHED || 
			    alb.mask[j] == MATCHED)
			{
				break;
			}
			
			count++;
			be++;
			ae++;
			
			if (ae > (int)ala.seq.size())
			{
				ae--;
				break;
			}
		}
		
		if (count > best_score)
		{
			best_score = count;
			bstart = bs;
			astart = as;
			length = ae - as;
			
			double my_pct = (i - low) / (high - low);
			best_pct_diff = fabs(my_pct - pct);
		}
		else if (count == best_score)
		{
			double my_pct = (i - low) / (high - low);
			double pct_diff = fabs(my_pct - pct);
			
			if (pct_diff < best_pct_diff)
			{
				best_score = count;
				bstart = bs;
				astart = as;
				length = ae - as;
				best_pct_diff = pct_diff;
			}
		}
	}
	
	if (best_score < threshold)
	{
		ala.mask[pos] = threshold;
		return 0;
	}
	
	for (size_t i = astart; i < astart + length; i++)
	{
		ala.mask[i] = MATCHED;
		ala.map[i] = i - (astart - bstart);
	}

	for (size_t i = bstart; i < bstart + length; i++)
	{
		alb.mask[i] = MATCHED;
		alb.map[i] = i - (bstart - astart);
	}
	
	return best_score;
}

void print_masks(Alignment &al)
{
	for (size_t i = 0; i < al.seq.length(); i++)
	{
		if ((int)al.mask[i] == MATCHED)
		{
			std::cout << ".";
		}
		else
		{
			std::cout << (int)al.mask[i];
		}
	}
	std::cout << std::endl;
}

void print_map(Alignment &al)
{
	for (size_t i = 0; i < al.seq.length(); i++)
	{
		if (al.map[i] == std::string::npos)
		{
			std::cout << ".";
		}
		else
		{
			std::cout << " " << (int)al.map[i] << " ";
		}
	}
	std::cout << std::endl;
}

void loop_alignment(Alignment &ala, Alignment &alb)
                    
{
	int count = 0;
	int threshold = 6;

	while (true)
	{
		int aseed = choose_seed(ala, threshold);

		if (aseed < 0)
		{
			break;
		}

		int match = best_match(ala, alb, aseed, threshold);
		
		count++;

		if (match == 0)
		{
			int more = false;
			for (size_t i = 0; i < ala.seq.length(); i++)
			{
				if (ala.mask[i] == UNTOUCHED || 
				    ala.mask[i] > (char)threshold)
				{
					more = true;
				}
			}
			
			if (!more)
			{
				threshold--;
				if (threshold < 0)
				{
					break;
				}
			}
		}
	}
}

void score_alignment(Alignment ala, Alignment alb,
                     int *muts, int *dels)
{
	*dels = abs(ala.seq.length() - alb.seq.length());
	
	int total = 0;
	for (size_t i = 0; i < ala.seq.length(); i++)
	{
		if (ala.mask[i] != MATCHED)
		{
			total++;
		}
	}

	for (size_t i = 0; i < alb.seq.length(); i++)
	{
		if (alb.mask[i] != MATCHED)
		{
			total++;
		}
	}
	
	total -= *dels;
	total /= 2;
	*muts = total;
}

void setup_alignment(Alignment *ala, std::string a)
{
	ala->seq = a;
	ala->mask = new char[a.length()];
	ala->map = new size_t[a.length()];
	memset(ala->mask, UNTOUCHED, ala->seq.length() * sizeof(char));
	
	for (size_t i = 0; i < a.length(); i++)
	{
		ala->map[i] = std::string::npos;
	}
}

inline void compare_sequences(std::string a, std::string b,
                              int *muts, int *dels)
{
	int best_mut = a.length();
	std::cout << a << std::endl;
	std::cout << b << std::endl;
	int improved = 0;
	Alignment besta, bestb;

	for (size_t i = 0; i < 4; i++)
	{
		int mut;
	
		Alignment ala, alb;
		setup_alignment(&ala, a);
		setup_alignment(&alb, b);

		loop_alignment(ala, alb);
		score_alignment(ala, alb, &mut, dels);
		
		if (mut < best_mut)
		{
			improved++;
			best_mut = mut;
			besta = ala;
			bestb = alb;
		}
	}
	
	std::cout << "Improved " << improved - 1 << " times." << std::endl;
	print_masks(besta);
	print_masks(bestb);
	
	*muts = best_mut;
	
	std::cout << "Muts, dels: " << *muts << " " << *dels << std::endl;
	std::cout << std::endl;
}

#endif

