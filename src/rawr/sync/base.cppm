export module rawr.sync.base;
import        rawr.lib.integer.raw;

export namespace rawr::sync::inline base
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
