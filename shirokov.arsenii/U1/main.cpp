#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>

namespace shirokov
{
  struct Person
  {
    size_t id;
    std::string info;
  };

  void makeIn(const std::string&, std::ifstream&, std::istream**);
  void makeOut(const std::string&, std::ofstream&, std::ostream**);
  void expand(Person**, size_t, size_t&);
  bool contains(const Person&, const Person*, size_t);
}

int main(int argc, char* argv[])
{
  std::istream* inPtr = &std::cin;
  std::ifstream fileIn;
  std::ostream* outPtr = &std::cout;
  std::ofstream fileOut;
  std::string outFilename;
  bool inStatus = false, outStatus = false;

  if (argc > 1)
  {
    std::string filename = argv[1];
    if (filename.compare(0, 3, "in:") == 0)
    {
      try
      {
        shirokov::makeIn(filename.substr(3), fileIn, &inPtr);
      }
      catch (const std::runtime_error& e)
      {
        std::cerr << e.what() << '\n';
        return 1;
      }
      inStatus = true;
    }
    else if (filename.compare(0, 4, "out:") == 0)
    {
      outFilename = filename.substr(4);
      outStatus = true;
    }
    else
    {
      std::cerr << "Invalid args" << '\n';
      return 1;
    }
  }

  if (argc > 2)
  {
    std::string filename = argv[2];
    if (filename.compare(0, 3, "in:") == 0)
    {
      if (inStatus)
      {
        std::cerr << "Input file already exist" << '\n';
        return 1;
      }
      try
      {
        shirokov::makeIn(filename.substr(3), fileIn, &inPtr);
      }
      catch (const std::runtime_error& e)
      {
        std::cerr << e.what() << '\n';
        return 1;
      }
    }
    else if (filename.compare(0, 4, "out:") == 0)
    {
      if (outStatus)
      {
        std::cerr << "Output file already exist" << '\n';
        return 2;
      }
      outFilename = filename.substr(4);
      outStatus = true;
    }
    else
    {
      std::cerr << "Invalid args" << '\n';
      return 1;
    }
  }

  std::istream& in = *inPtr;

  shirokov::Person* massive = nullptr;
  size_t size = 0, cap = 0;
  size_t fail = 0;

  size_t id = 0;
  while (in >> id)
  {
    while (in.peek() == ' ' || in.peek() == '\t')
    {
      in.get();
    }

    std::string info;

    if (!std::getline(in, info))
    {
      if (!in.eof())
      {
        in.clear();
        in.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
        continue;
      }
      break;
      ++fail;
    }

    if (info.empty())
    {
      ++fail;
      continue;
    }

    shirokov::Person person{id, info};

    if (shirokov::contains(person, massive, size))
    {
      ++fail;
      continue;
    }

    if (size == cap)
    {
      try
      {
        shirokov::expand(&massive, size, cap);
      }
      catch (const std::bad_alloc&)
      {
        std::cerr << "Bad alloc" << '\n';
        delete[] massive;
        return 3;
      }
    }
    massive[size++] = shirokov::Person{id, info};
  }

  if (!in && !in.eof())
  {
    std::cerr << "Input format error occurred" << '\n';
    delete[] massive;
    return 1;
  }

  if (outStatus)
  {
    try
    {
      shirokov::makeOut(outFilename, fileOut, &outPtr);
    }
    catch (const std::runtime_error& e)
    {
      std::cerr << e.what() << '\n';
      delete[] massive;
      return 2;
    }
  }

  std::ostream& out = *outPtr;

  for (size_t i = 0; i < size; ++i)
  {
    out << massive[i].id << ' ' << massive[i].info << '\n';
  }
  if (!size)
  {
    out << '\n';
  }

  std::cerr << size << ' ' << fail << '\n';

  delete[] massive;
}

void shirokov::makeIn(const std::string& filename, std::ifstream& fileIn, std::istream** inPtr)
{
  if (fileIn.is_open())
  {
    fileIn.close();
    fileIn.clear();
  }
  fileIn.open(filename);
  if (fileIn.is_open())
  {
    *inPtr = &fileIn;
  }
  else
  {
    throw std::runtime_error("Couldn't open the input file");
  }
}

void shirokov::makeOut(const std::string& filename, std::ofstream& fileOut, std::ostream** outPtr)
{
  if (fileOut.is_open())
  {
    fileOut.close();
    fileOut.clear();
  }
  fileOut.open(filename);
  if (fileOut.is_open())
  {
    *outPtr = &fileOut;
  }
  else
  {
    throw std::runtime_error("Couldn't open the output file");
  }
}

void shirokov::expand(Person** massive, size_t size, size_t& cap)
{
  size_t newCap = (cap == 0 ? 1 : cap * 2);
  Person* newMassive = new Person[newCap];

  for (size_t i = 0; i < size; ++i)
  {
    newMassive[i] = (*massive)[i];
  }

  delete[] *massive;
  *massive = newMassive;
  cap = newCap;
}

bool shirokov::contains(const Person& person, const Person* massive, size_t size)
{
  for (size_t i = 0; i < size; ++i)
  {
    if (massive[i].id == person.id)
    {
      return true;
    }
  }
  return false;
}
