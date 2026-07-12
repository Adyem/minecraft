#ifndef I_VALIDATOR_HPP
# define I_VALIDATOR_HPP

class IValidator
{
    public:
        virtual ~IValidator() {}
        virtual int validate() const = 0;
};

#endif
