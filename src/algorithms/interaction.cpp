 
#include "interaction.h"

#include <iostream>

#include "madara/knowledge/containers/String.h"
#include "gams/groups/GroupFactoryRepository.h"

gams::algorithms::BaseAlgorithm *
algorithms::interactionFactory::create (
  const madara::knowledge::KnowledgeMap & /*args*/,
  madara::knowledge::KnowledgeBase * knowledge,
  gams::platforms::BasePlatform * platform,
  gams::variables::Sensors * sensors,
  gams::variables::Self * self,
  gams::variables::Agents * agents)
{
  gams::algorithms::BaseAlgorithm * result (0);

  if (knowledge && sensors && platform && self)
  {
    result = new interaction (knowledge, platform, sensors, self);
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
      "algorithms::interactionFactory::create:"
      " failed to create due to invalid pointers. "
      " knowledge=%p, sensors=%p, platform=%p, self=%p, agents=%p\n",
      knowledge, sensors, platform, self, agents);
  }

  /**
   * Note the usage of logger macros with the GAMS global logger. This
   * is highly optimized and is just an integer check if the log level is
   * not high enough to print the message
   **/
  if (result == 0)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
      "algorithms::interactionFactory::create:"
      " unknown error creating interaction algorithm\n");
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
      "algorithms::interactionFactory::create:"
      " successfully created interaction algorithm\n");
  }

  return result;
}

algorithms::interaction::interaction (
  madara::knowledge::KnowledgeBase * knowledge,
  gams::platforms::BasePlatform * platform,
  gams::variables::Sensors * sensors,
  gams::variables::Self * self,
  gams::variables::Agents * agents)
  : gams::algorithms::BaseAlgorithm (knowledge, platform, sensors, self, agents)
{
  status_.init_vars (*knowledge, "interaction", self->agent.prefix);
  status_.init_variable_values ();

  if (knowledge)
  {
    gams::groups::GroupFactoryRepository factory(knowledge);
    m_group = factory.create("group.allies");
  }
  else
  {
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_MAJOR,
        "algorithms::interaction::interaction:"
        " interaction constructor is not passed completely, knowledge is null\n");
  }
}

algorithms::interaction::~interaction ()
{
}

int
algorithms::interaction::analyze (void)
{
  //need to find the key for ally instead of this
  std::string ally_id = knowledge_->get(".id").to_string() == "0" ? "1" : "0";
  m_group->sync();
  m_group->get_members(m_allies);
  if (knowledge_)
  {
    // we expect exactly 2 nodes
    if (m_allies.size () == 2)
    {
      std::string key = ally_id + "myKey";


      //string value to print value received by key
      madara::knowledge::containers::String string_value(key, *knowledge_);
      std::cout << *string_value << "\n\n\n";
    }
  }
  return 0;
}


int
algorithms::interaction::execute (void)
{
  if (count == 20)
  {
    // after 20 dump knowledge
    knowledge_->save_as_json(std::string("kb.json"));
    ++count;
    return 0;
  }
  else if (count > 21)
  {
    // do not send any message
    return 1;
  }
  //regular case
  std::string key = knowledge_->get(".id").to_string() + "myKey";
  knowledge_->set(key, "writing some string with counting number " + std::to_string(knowledge_->get(".id").to_integer() * count++));
  knowledge_->send_modifieds();
  return 0;
}


int
algorithms::interaction::plan (void)
{
  return 0;
}
