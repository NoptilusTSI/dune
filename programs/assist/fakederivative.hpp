//***************************************************************************
// Copyright 2007-2014 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Universidade do Porto. For licensing   *
// terms, conditions, and further information contact lsts@fe.up.pt.        *
//                                                                          *
// European Union Public Licence - EUPL v.1.1 Usage                         *
// Alternatively, this file may be used under the terms of the EUPL,        *
// Version 1.1 only (the "Licence"), appearing in the file LICENCE.md       *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://www.lsts.pt/dune/licence.                                        *
//***************************************************************************
// Author: José Braga                                                       *
//***************************************************************************

#ifndef FAKEDERIVATIVE_HPP_INCLUDED_
#define FAKEDERIVATIVE_HPP_INCLUDED_

// ISO C++ 98 headers.
#include <cmath>

// DUNE headers.
#include <DUNE/Config.hpp>

#include "fakedelta.hpp"

namespace DUNE
{
  namespace Math
  {
    // %Derivative applies Newton's difference quotient.
    template <typename Type>
    class FakeDerivative
    {
    public:
      //! Constructor.
      FakeDerivative(void)
      {
        clear();
      }

      //! Clear data.
      void
      clear(void)
      {
        m_val_latest = 0;
        m_val_previous = 0;
        m_derivative = 0;
        m_delta.clear();
      }

      //! Update with new value.
      Type
      update(const Type& value, double time)
      {
        m_val_previous = m_val_latest;
        m_val_latest = value;

        double tstep = m_delta.getDelta(time);

        if (tstep <= 0.0)
          return 0;

        m_derivative = (m_val_latest - m_val_previous) / tstep;

        return m_derivative;
      }

      //! Check current derivative.
      Type
      check(void) const
      {
        return m_derivative;
      }

    private:
      //! Latest value.
      Type m_val_latest;
      //! Previous value.
      Type m_val_previous;
      //! Derivative.
      Type m_derivative;
      //! Time window between values.
      FakeDelta m_delta;
    };
  }
}

#endif
