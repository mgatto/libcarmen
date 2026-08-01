#include <stddef.h>
#include "carmen/artifact.h"

const CarmenArtifact CARMEN_ARTIFACTS[CARMEN_ARTIFACT_COUNT] = {
    {"astrolabe_isfahan",
     "artifact.astrolabe_isfahan.name",
     "isfahan",
     "artifact.astrolabe_isfahan.desc"},

    {"quran_manuscript_cairo",
     "artifact.quran_manuscript_cairo.name",
     "cairo",
     "artifact.quran_manuscript_cairo.desc"},

    {"iznik_tile_istanbul",
     "artifact.iznik_tile_istanbul.name",
     "istanbul",
     "artifact.iznik_tile_istanbul.desc"},

    {"khanjar_muscat",
     "artifact.khanjar_muscat.name",
     "muscat",
     "artifact.khanjar_muscat.desc"},

    {"brass_lamp_cairo",
     "artifact.brass_lamp_cairo.name",
     "cairo",
     "artifact.brass_lamp_cairo.desc"},

    {"whirling_robe_konya",
     "artifact.whirling_robe_konya.name",
     "konya",
     "artifact.whirling_robe_konya.desc"},

    {"silk_paper_samarkand",
     "artifact.silk_paper_samarkand.name",
     "samarkand",
     "artifact.silk_paper_samarkand.desc"},

    {"mughal_miniature_lahore",
     "artifact.mughal_miniature_lahore.name",
     "lahore",
     "artifact.mughal_miniature_lahore.desc"},

    {"zellige_mosaic_fez",
     "artifact.zellige_mosaic_fez.name",
     "fez",
     "artifact.zellige_mosaic_fez.desc"},

    {"pearl_chest_dubai",
     "artifact.pearl_chest_dubai.name",
     "dubai",
     "artifact.pearl_chest_dubai.desc"},

    {"vakufnama_sarajevo",
     "artifact.vakufnama_sarajevo.name",
     "sarajevo",
     "artifact.vakufnama_sarajevo.desc"},

    {"zanzibar_door",
     "artifact.zanzibar_door.name",
     "zanzibar",
     "artifact.zanzibar_door.desc"},

    {"muslin_dhaka",
     "artifact.muslin_dhaka.name",
     "dhaka",
     "artifact.muslin_dhaka.desc"},

    {"pewter_tankard_kuala_lumpur",
     "artifact.pewter_tankard_kuala_lumpur.name",
     "kuala_lumpur",
     "artifact.pewter_tankard_kuala_lumpur.desc"},

    {"keris_jakarta",
     "artifact.keris_jakarta.name",
     "jakarta",
     "artifact.keris_jakarta.desc"},

    {"glass_weight_dakar",
     "artifact.glass_weight_dakar.name",
     "dakar",
     "artifact.glass_weight_dakar.desc"},

    {"ivory_casket_tunis",
     "artifact.ivory_casket_tunis.name",
     "tunis",
     "artifact.ivory_casket_tunis.desc"},

    {"damascene_sabre_damascus",
     "artifact.damascene_sabre_damascus.name",
     "damascus",
     "artifact.damascene_sabre_damascus.desc"},

    {"indigo_robe_kano",
     "artifact.indigo_robe_kano.name",
     "kano",
     "artifact.indigo_robe_kano.desc"},

    {"lapis_bowl_kabul",
     "artifact.lapis_bowl_kabul.name",
     "kabul",
     "artifact.lapis_bowl_kabul.desc"},

    {"batik_cloth_yogyakarta",
     "artifact.batik_cloth_yogyakarta.name",
     "yogyakarta",
     "artifact.batik_cloth_yogyakarta.desc"},
};

int carmen_artifact_count(void)
{
    return CARMEN_ARTIFACT_COUNT;
}

const CarmenArtifact *carmen_artifact_at(int index)
{
    if (index < 0 || index >= CARMEN_ARTIFACT_COUNT) return NULL;
    return &CARMEN_ARTIFACTS[index];
}
