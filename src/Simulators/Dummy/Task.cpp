//***************************************************************************
// Copyright 2007-2013 Universidade do Porto - Faculdade de Engenharia      *
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
// Author: Pedro Calado                                                     *
//***************************************************************************

// DUNE headers.
#include <DUNE/DUNE.hpp>

using DUNE_NAMESPACES;

namespace Simulators
{
  //! %Dummy simulator for DUNE.
  //! %Dummy task mimics basic parameters behavior
  //! such as activation, deactivation times
  //! and set entity parameter.
  //!
  //! @author Pedro Calado
  namespace Dummy
  {
    //! %Task arguments.
    struct Arguments
    {
      //! Actual activation time of the device
      float actual_act_time;
      //! Actual deactivation time of the device
      float actual_deact_time;
    };

    //! %Dummy simulator task
    struct Task: public Tasks::Periodic
    {
      //! Timer for activation process
      Time::Counter<float> m_act_timer;
      //! Timer for deactivation process
      Time::Counter<float> m_deact_timer;
      //! Activating
      bool m_activating;
      //! Deactivating
      bool m_deactivating;
      //! Task arguments.
      Arguments m_args;

      Task(const std::string& name, Tasks::Context& ctx):
        Tasks::Periodic(name, ctx),
        m_activating(false),
        m_deactivating(false)
      {
        param("Actual Activation Time", m_args.actual_act_time)
        .defaultValue("30.0")
        .description("Actual activation time of the device");

        param("Actual Deactivation Time", m_args.actual_deact_time)
        .defaultValue("10.0")
        .description("Actual deactivation time of the device");

        paramActive(Tasks::Parameter::SCOPE_MANEUVER,
                    Tasks::Parameter::VISIBILITY_USER);

        // Initialize entity state.
        setEntityState(IMC::EntityState::ESTA_NORMAL, Status::CODE_IDLE);
      }

      //! On update parameters
      void
      onUpdateParameters(void)
      {
        m_act_timer.setTop(m_args.actual_act_time);
        m_deact_timer.setTop(m_args.actual_deact_time);
      }

      //! On activation
      void
      onRequestActivation(void)
      {
        m_act_timer.reset();
        m_activating = true;
      }

      //! On deactivation
      void
      onRequestDeactivation(void)
      {
        m_deact_timer.reset();
        m_deactivating = true;
      }

      void
      task(void)
      {
        if (m_activating && m_act_timer.overflow())
        {
          m_activating = false;
          activate();
        }

        if (m_deactivating && m_deact_timer.overflow())
        {
          m_deactivating = false;
          deactivate();
        }
      }
    };
  }
}

DUNE_TASK
