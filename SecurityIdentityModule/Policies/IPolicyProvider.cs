using SecurityIdentityModule.Models;

namespace SecurityIdentityModule.Policies;

public interface IPolicyProvider
{
    SecurityPolicy GetCurrentPolicy();
}
