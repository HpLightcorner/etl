///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2020 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_BRESENHAM_LINE_INCLUDED
#define ETL_BRESENHAM_LINE_INCLUDED

#include <stddef.h>

#include "platform.h"
#include "iterator.h"
#include "exception.h"

namespace etl
{
  //***************************************************************************
  /// A pseudo-container that generates points on a line, using Bresenham's
  /// line algorithm.
  /// TCoordinate must support integral x & y members.
  /// TWorking is the internal working variable type. Default is int.
  //***************************************************************************
  template <typename TCoordinate, typename TWorking = int>
  class bresenham_line
  {
  public:

    //***************************************************
    /// Standard container types.
    //***************************************************
    typedef TCoordinate       value_type;
    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;

    class iterator : public etl::iterator<ETL_OR_STD::forward_iterator_tag, TCoordinate>
    {
    public:

      friend class bresenham_line;

      //***************************************************
      /// Default constructor
      //***************************************************
      iterator()
        : p_bresenham_line(ETL_NULLPTR)
      {
      }

      //***************************************************
      /// Copy constuctor
      //***************************************************
      iterator(const iterator& other)
        : p_bresenham_line(other.p_bresenham_line)
      {
      }

      //***************************************************
      /// Assignment operator
      //***************************************************
      iterator& operator =(const iterator& rhs)
      {
        p_bresenham_line = rhs.p_bresenham_line;

        return *this;
      }

      //***************************************************
      /// Pre-increment operator
      //***************************************************
      iterator& operator ++()
      {
        // Has the end of the series has been reached?
        if (p_bresenham_line->n_coordinates_remaining == TWorking(0))
        {
          // Mark it as an end iterator.
          p_bresenham_line = ETL_NULLPTR;
        }
        else
        {
          p_bresenham_line->next();
        }

        return *this;
      }

      //***************************************************
      /// De-reference operator
      //***************************************************
      value_type operator *() const
      {
        return p_bresenham_line->get_coordinate();
      }

      //***************************************************
      /// Equality operator
      //***************************************************
      friend bool operator ==(const iterator& lhs, const iterator& rhs)
      {
        return lhs.p_bresenham_line == rhs.p_bresenham_line;
      }

      //***************************************************
      /// Inequality operator
      //***************************************************
      friend bool operator !=(const iterator& lhs, const iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      //***************************************************
      /// Constructor for use by bresenham_line
      //***************************************************
      iterator(bresenham_line<TCoordinate, TWorking>* pb)
        : p_bresenham_line(pb)
      {
      }

      bresenham_line<TCoordinate, TWorking>* p_bresenham_line;
    };

    //***************************************************
    /// Constructor.
    /// Supplied first and last coordinates
    //***************************************************
    bresenham_line(const_reference first_, const_reference last_)
      : first(first_)
      , coordinate(first_)
      , x_increment((last_.x < first_.x) ? -1 : 1)
      , y_increment((last_.y < first_.y) ? -1 : 1)
      , dx((last_.x < first_.x) ? first_.x - last_.x : last_.x - first_.x)
      , dy((last_.y < first_.y) ? first_.y - last_.y : last_.y - first_.y)
      , do_minor_increment(false)
    {
      if (is_y_major_axis())
      {
        total_n_coordinates = dy + 1;
        dx *= 2;
        balance = dx - dy;
        dy *= 2;
      }
      else
      {
        total_n_coordinates = dx + 1;
        dy *= 2;
        balance = dy - dx;
        dx *= 2;
      }

      n_coordinates_remaining = total_n_coordinates - 1; // We already have the first coordinate.
    }

    //***************************************************
    /// Get an iterator to the first coordinate.
    /// Resets the Bresenham line.
    //***************************************************
    iterator begin()
    {
      n_coordinates_remaining = total_n_coordinates - 1; // We already have the first coordinate.
      coordinate = first;

      return iterator(this);
    }

    //***************************************************
    /// Get an iterator to one past the last coordinate.
    //***************************************************
    iterator end() const
    {
      return iterator();
    }

    //***************************************************
    /// Get the size of the series.
    //***************************************************
    size_t size() const
    {
      return total_n_coordinates;
    }

    //***************************************************
    /// Get the current number of generated points.
    //***************************************************
    size_t count() const
    {
      return total_n_coordinates - n_coordinates_remaining;
    }

  private:

    //***************************************************
    /// Returns true if Y is the major axis.
    //***************************************************
    int is_y_major_axis() const
    {
      return dx < dy;
    }

    //***************************************************
    /// Calculate the next point.
    //***************************************************
    void next()
    {
      if (is_y_major_axis())
      {
        // Y major axis.
        if (do_minor_increment)
        {
          coordinate.x += x_increment;
          balance -= dy;
        }

        coordinate.y += y_increment;
        balance += dx;
      }
      else
      {
        // X major axis.
        if (do_minor_increment)
        {
          coordinate.y += y_increment;
          balance -= dx;
        }

        coordinate.x += x_increment;
        balance += dy;
      }

      --n_coordinates_remaining;

      do_minor_increment = (balance >= 0);
    }

    //***************************************************
    /// Get the current coordinate.
    //***************************************************
    value_type get_coordinate() const
    {
      return coordinate;
    }

    typedef TWorking working_t;

    const value_type  first;
    value_type        coordinate;
    const working_t   x_increment;
    const working_t   y_increment;
    working_t         dx;
    working_t         dy;
    working_t         total_n_coordinates;
    working_t         n_coordinates_remaining;
    working_t         balance;
    bool              do_minor_increment;
  };
}

#endif

