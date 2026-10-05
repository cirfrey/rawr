export module rawr.lib.sync.base;
import        rawr.lib.integer.raw;

export namespace rawr::inline lib::inline sync
{
    enum class memory_order : ru8
    {
        relaxed,
        acquire,
        release,
        acq_rel,
        seq_cst,
    };
}
