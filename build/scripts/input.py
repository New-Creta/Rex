def ask(question):
  print(question)
  return input()

def affirmative_response(response : str):
  return response.lower() == "y" or response.lower() == "yes"