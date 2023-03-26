
class logger
{
public:
    logger(const std::string& file) :
        f()
    {
        f.open(file);
    }

    ~logger()
    {
        f.close();
    }

    void write(const std::string& line)
    {
        f << line << std::endl;
    }

private:
    std::ofstream f;
};
