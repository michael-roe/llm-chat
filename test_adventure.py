
import adventure

scenario_data = '{"scenario": "pinecrest"}'

scenario_object = adventure.InitializeRoleplay(scenario='pinecrest')

print(scenario_object.model_dump())

print(scenario_object())

update_object = adventure.UpdateCharacters(
  character='Michael', location='cave')

print(update_object.model_dump())

update_object()
